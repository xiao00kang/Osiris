#include "../Config.h"
#include "../Interfaces.h"
#include "../Memory.h"
#include "../Netvars.h"
#include "Misc.h"

void Misc::bunnyHop(UserCmd* cmd) noexcept
{
    constexpr int ON_GROUND{ 1 };
    constexpr int IN_JUMP{ 2 };
    if (config.misc.bunnyHop) {
        if (cmd->buttons & IN_JUMP) {
            auto localPlayer = interfaces.entityList->getClientEntity(interfaces.engine->getLocalPlayer());
            if (*localPlayer->getFlags() & ON_GROUND)
                cmd->buttons |= IN_JUMP;
            else
                cmd->buttons &= ~IN_JUMP;
        }
    }
}

void Misc::disablePostProcessing() noexcept
{
    if (*memory.disablePostProcessing != config.misc.disablePostProcessing)
        *memory.disablePostProcessing = config.misc.disablePostProcessing;
}

void Misc::skybox() noexcept
{
    constexpr std::string_view skyboxes[]{ "Default", "cs_baggage_skybox_", "cs_tibet", "embassy", "italy", "jungle", "nukeblank", "office", "sky_cs15_daylight01_hdr", "sky_cs15_daylight02_hdr", "sky_cs15_daylight03_hdr", "sky_cs15_daylight04_hdr", "sky_csgo_cloudy01", "sky_csgo_night_flat", "sky_csgo_night02", "sky_day02_05_hdr", "sky_day02_05", "sky_dust", "sky_l4d_rural02_ldr", "sky_venice", "vertigo_hdr", "vertigo", "vertigoblue_hdr", "vietnam" };

    if (config.misc.skybox)
        reinterpret_cast<void(__fastcall*)(const char*)>(memory.loadSky)(skyboxes[config.misc.skybox].data());
}

void Misc::clanTag(const std::string& clanTag) noexcept
{
    reinterpret_cast<void(__fastcall*)(const char*, const char*)>(memory.setClanTag)(clanTag.c_str(), clanTag.c_str());
}

void Misc::colorWorld() noexcept
{
    static auto red = interfaces.cvar->findVar("mat_ambient_light_r");
    static auto green = interfaces.cvar->findVar("mat_ambient_light_g");
    static auto blue = interfaces.cvar->findVar("mat_ambient_light_b");

    red->setValue(config.misc.worldColor[0]);
    green->setValue(config.misc.worldColor[1]);
    blue->setValue(config.misc.worldColor[2]);
}

void Misc::inverseRagdollGravity() noexcept
{
    static auto ragdollGravity = interfaces.cvar->findVar("cl_ragdoll_gravity");
    ragdollGravity->setValue(config.misc.inverseRagdollGravity ? -600 : 600);
}

void Misc::reduceFlashEffect() noexcept
{
    auto localPlayer = interfaces.entityList->getClientEntity(interfaces.engine->getLocalPlayer());
    *reinterpret_cast<float*>(localPlayer + netvars.getOffset("m_flFlashMaxAlpha")) = 255.0f - config.misc.flashReduction * 2.55f;
}

void Misc::removeCrouchCooldown(UserCmd* cmd) noexcept
{
    if (config.misc.fastDuck) {
        constexpr int IN_BULLRUSH{ 4194304 };
        cmd->buttons |= IN_BULLRUSH;
    }
}

void Misc::removeSmoke() noexcept
{
    constexpr std::string_view smokeMaterials[]{
        "particle/vistasmokev1/vistasmokev1_emods",
        "particle/vistasmokev1/vistasmokev1_emods_impactdust",
        "particle/vistasmokev1/vistasmokev1_fire",
        "particle/vistasmokev1/vistasmokev1_smokegrenade",
    };

    for (const auto mat : smokeMaterials) {
        auto material = interfaces.materialSystem->findMaterial(mat);
        material->setMaterialVarFlag(MaterialVar::NO_DRAW, config.misc.noSmoke);
        material->setMaterialVarFlag(MaterialVar::WIREFRAME, config.misc.wireframeSmoke);
    }

    if (config.misc.noSmoke || config.misc.wireframeSmoke)
        *memory.smokeCount = 0;
}

void Misc::autoPistol(UserCmd* cmd) noexcept
{
    if (config.misc.autoPistol) {
        const auto activeWeapon = interfaces.entityList->getClientEntityFromHandle(
            interfaces.entityList->getClientEntity(
                interfaces.engine->getLocalPlayer())->getActiveWeaponHandle());
        if (activeWeapon && activeWeapon->isPistol()) {
            static bool wasInAttackLastTick{ false };
            if (cmd->buttons & 1 && wasInAttackLastTick)
                cmd->buttons &= ~1;
            wasInAttackLastTick = cmd->buttons & 1;
        }
    }
}
