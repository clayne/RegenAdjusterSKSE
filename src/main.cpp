﻿#include "Project/RegenAdjuster.h"
#include "Project/Hooks/Hooks.h"

const SKSE::MessagingInterface* g_messaging2 = nullptr;

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Query(const SKSE::QueryInterface * a_skse, SKSE::PluginInfo * a_info)
{
#ifndef NDEBUG
    auto sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
#else
    auto path = logger::log_directory();
    if (!path) {
        return false;
    }

    *path /= "loki_POISE.log"sv;
    auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
#endif

    auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));

#ifndef NDEBUG
    log->set_level(spdlog::level::trace);
#else
    log->set_level(spdlog::level::info);
    log->flush_on(spdlog::level::info);
#endif

    spdlog::set_default_logger(std::move(log));
    spdlog::set_pattern("%g(%#): [%^%l%$] %v"s);

    logger::info("loki_RegenAdjuster v1.0.0");

    a_info->infoVersion = SKSE::PluginInfo::kVersion;
    a_info->name = "loki_RegenAdjuster";
    a_info->version = 1;

    if (a_skse->IsEditor()) {
        logger::critical("Loaded in editor, marking as incompatible"sv);
        return false;
    }

    const auto ver = a_skse->RuntimeVersion();
    if (ver < SKSE::RUNTIME_1_5_39) {
        logger::critical(FMT_STRING("Unsupported runtime version {}"), ver.string());
        return false;
    }

    return true;
}

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface * a_skse)
{
    logger::info("loki_RegenAdjuster loaded");
    SKSE::Init(a_skse);
    SKSE::AllocTrampoline(64);

    Loki::Hooks::InstallActorUpdateHook();

    return true;
}