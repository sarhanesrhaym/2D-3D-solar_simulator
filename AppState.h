#pragma once

// États de l'application
enum class AppState {
    MAIN_MENU,
    SIMULATION,
    PRESENTATION,
    TEAM_INFO,
    PLANET_DETAILS,
    PLANET_STRUCTURE    // ✅ NOUVEAU : Structure interne des planètes
};