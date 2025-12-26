#include "theme.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gtk/gtk.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>
#include <gdk/gdkpixbuf.h>

#include <cairo.h>
#include <pango/pango.h>
#include <pango/pangocairo.h>

#include <errno.h>
#include <ctype.h>
#include <math.h>

// Modern color scheme based on Student Login design
#define MODERN_BG_LIGHT_BLUE "#E3F2FD"
#define MODERN_CARD_WHITE "#FFFFFF"
#define MODERN_PRIMARY_BLUE "#2196F3"
#define MODERN_PRIMARY_DARK_BLUE "#1976D2"
#define MODERN_TEXT_DARK "#1A1A1A"
#define MODERN_TEXT_GRAY "#666666"
#define MODERN_TEXT_LIGHT_GRAY "#999999"
#define MODERN_BORDER_LIGHT "#E0E0E0"
#define MODERN_SHADOW "rgba(0, 0, 0, 0.1)"
#define MODERN_SHADOW_HOVER "rgba(0, 0, 0, 0.15)"
#define MODERN_PINK_ACCENT "#FF4081"

// ============================================================================
// THEME CONFIGURATION MANAGEMENT
// ============================================================================

ThemeConfig* theme_config_create(void) {
    ThemeConfig* config = (ThemeConfig*)calloc(1, sizeof(ThemeConfig));
    if (!config) return NULL;
    
    // Initialize with modern defaults
    config->current_theme = THEME_LIGHT;
    config->font_size = THEME_DEFAULT_FONT_SIZE;
    config->font_size_large = THEME_DEFAULT_FONT_SIZE_LARGE;
    config->font_size_small = THEME_DEFAULT_FONT_SIZE_SMALL;
    config->border_radius = 12; // Modern rounded corners
    config->padding_small = 8;
    config->padding_medium = 16;
    config->padding_large = 24;
    config->margin_small = 8;
    config->margin_medium = 16;
    config->margin_large = 24;
    config->animation_duration = 250;
    config->enable_animations = 1;
    config->enable_transparency = 0;
    config->transparency_level = 1.0;
    
    strncpy(config->font_family, "Inter, Roboto, -apple-system, sans-serif", sizeof(config->font_family) - 1);
    
    // Initialize light colors (modern Student Login theme)
    ColorScheme* light = theme_create_light_colors();
    if (light) {
        memcpy(&config->light_colors, light, sizeof(ColorScheme));
        free(light);
    }
    
    // Initialize dark colors
    ColorScheme* dark = theme_create_dark_colors();
    if (dark) {
        memcpy(&config->dark_colors, dark, sizeof(ColorScheme));
        free(dark);
    }
    
    return config;
}

void theme_config_destroy(ThemeConfig* config) {
    if (config) {
        free(config);
    }
}

int theme_config_load(ThemeConfig* config, const char* config_file) {
    if (!config || !config_file) return 0;
    // TODO: Implement config file loading
    return 1;
}

int theme_config_save(ThemeConfig* config, const char* config_file) {
    if (!config || !config_file) return 0;
    // TODO: Implement config file saving
    return 1;
}

int theme_config_set_theme(ThemeConfig* config, ThemeType theme) {
    if (!config) return 0;
    if (theme < THEME_LIGHT || theme > THEME_AUTO) return 0;
    config->current_theme = theme;
    return 1;
}

// ============================================================================
// COLOR MANAGEMENT
// ============================================================================

ColorScheme* theme_create_light_colors(void) {
    ColorScheme* scheme = (ColorScheme*)calloc(1, sizeof(ColorScheme));
    if (!scheme) return NULL;
    
    strncpy(scheme->name, "Modern Light", sizeof(scheme->name) - 1);
    strncpy(scheme->background_color, MODERN_BG_LIGHT_BLUE, sizeof(scheme->background_color) - 1);
    strncpy(scheme->foreground_color, MODERN_TEXT_DARK, sizeof(scheme->foreground_color) - 1);
    strncpy(scheme->primary_color, MODERN_PRIMARY_BLUE, sizeof(scheme->primary_color) - 1);
    strncpy(scheme->secondary_color, MODERN_PRIMARY_DARK_BLUE, sizeof(scheme->secondary_color) - 1);
    strncpy(scheme->accent_color, MODERN_PINK_ACCENT, sizeof(scheme->accent_color) - 1);
    strncpy(scheme->error_color, "#F44336", sizeof(scheme->error_color) - 1);
    strncpy(scheme->warning_color, "#FF9800", sizeof(scheme->warning_color) - 1);
    strncpy(scheme->success_color, "#4CAF50", sizeof(scheme->success_color) - 1);
    strncpy(scheme->info_color, MODERN_PRIMARY_BLUE, sizeof(scheme->info_color) - 1);
    strncpy(scheme->border_color, MODERN_BORDER_LIGHT, sizeof(scheme->border_color) - 1);
    strncpy(scheme->selection_color, "#E3F2FD", sizeof(scheme->selection_color) - 1);
    strncpy(scheme->hover_color, "#BBDEFB", sizeof(scheme->hover_color) - 1);
    strncpy(scheme->disabled_color, "#E0E0E0", sizeof(scheme->disabled_color) - 1);
    
    return scheme;
}

ColorScheme* theme_create_dark_colors(void) {
    ColorScheme* scheme = (ColorScheme*)calloc(1, sizeof(ColorScheme));
    if (!scheme) return NULL;
    
    strncpy(scheme->name, "Modern Dark", sizeof(scheme->name) - 1);
    strncpy(scheme->background_color, "#121212", sizeof(scheme->background_color) - 1);
    strncpy(scheme->foreground_color, "#FFFFFF", sizeof(scheme->foreground_color) - 1);
    strncpy(scheme->primary_color, "#64B5F6", sizeof(scheme->primary_color) - 1);
    strncpy(scheme->secondary_color, "#42A5F5", sizeof(scheme->secondary_color) - 1);
    strncpy(scheme->accent_color, "#FF4081", sizeof(scheme->accent_color) - 1);
    strncpy(scheme->error_color, "#EF5350", sizeof(scheme->error_color) - 1);
    strncpy(scheme->warning_color, "#FFA726", sizeof(scheme->warning_color) - 1);
    strncpy(scheme->success_color, "#66BB6A", sizeof(scheme->success_color) - 1);
    strncpy(scheme->info_color, "#42A5F5", sizeof(scheme->info_color) - 1);
    strncpy(scheme->border_color, "#424242", sizeof(scheme->border_color) - 1);
    strncpy(scheme->selection_color, "#1E1E1E", sizeof(scheme->selection_color) - 1);
    strncpy(scheme->hover_color, "#2C2C2C", sizeof(scheme->hover_color) - 1);
    strncpy(scheme->disabled_color, "#424242", sizeof(scheme->disabled_color) - 1);
    
    return scheme;
}

ColorScheme* theme_create_custom_colors(const char* name) {
    if (!name) return NULL;
    ColorScheme* scheme = (ColorScheme*)calloc(1, sizeof(ColorScheme));
    if (!scheme) return NULL;
    strncpy(scheme->name, name, sizeof(scheme->name) - 1);
    return scheme;
}

int theme_set_color(ColorScheme* scheme, const char* color_name, const char* color_value) {
    if (!scheme || !color_name || !color_value) return 0;
    if (!theme_validate_color(color_value)) return 0;
    
    if (strcmp(color_name, "background") == 0) {
        strncpy(scheme->background_color, color_value, sizeof(scheme->background_color) - 1);
    } else if (strcmp(color_name, "foreground") == 0) {
        strncpy(scheme->foreground_color, color_value, sizeof(scheme->foreground_color) - 1);
    } else if (strcmp(color_name, "primary") == 0) {
        strncpy(scheme->primary_color, color_value, sizeof(scheme->primary_color) - 1);
    } else if (strcmp(color_name, "secondary") == 0) {
        strncpy(scheme->secondary_color, color_value, sizeof(scheme->secondary_color) - 1);
    } else if (strcmp(color_name, "accent") == 0) {
        strncpy(scheme->accent_color, color_value, sizeof(scheme->accent_color) - 1);
    }
    // Add more color mappings as needed
    return 1;
}

const char* theme_get_color(ColorScheme* scheme, const char* color_name) {
    if (!scheme || !color_name) return NULL;
    
    if (strcmp(color_name, "background") == 0) return scheme->background_color;
    if (strcmp(color_name, "foreground") == 0) return scheme->foreground_color;
    if (strcmp(color_name, "primary") == 0) return scheme->primary_color;
    if (strcmp(color_name, "secondary") == 0) return scheme->secondary_color;
    if (strcmp(color_name, "accent") == 0) return scheme->accent_color;
    
    return NULL;
}

int theme_validate_color(const char* color) {
    if (!color) return 0;
    // Check if it's a valid hex color (#RRGGBB or #RRGGBBAA)
    if (color[0] != '#') return 0;
    int len = strlen(color);
    if (len != 7 && len != 9) return 0;
    for (int i = 1; i < len; i++) {
        if (!isxdigit(color[i])) return 0;
    }
    return 1;
}

// ============================================================================
// CSS GENERATION - ULTRA MODERN WITH FLEXBOX
// ============================================================================

char* theme_generate_css(ThemeConfig* config) {
    if (!config) return NULL;
    
    ColorScheme* colors = (config->current_theme == THEME_DARK) ? 
                          &config->dark_colors : &config->light_colors;
    
    char* css = (char*)malloc(80000);
    if (!css) return NULL;
    
    snprintf(css, 80000,
        "/* Modern Student Management System Theme */\n\n"
        
        "* {\n"
        "    background-color: transparent;\n"
        "}\n\n"
        
        "window {\n"
        "    background: linear-gradient(145deg, #1976D2 0%%, #1E88E5 35%%, #2196F3 70%%, #42A5F5 100%%);\n"
        "    color: #1A1A1A;\n"
        "}\n\n"
        
        "button {\n"
        "    background-color: #2196F3;\n"
        "    color: white;\n"
        "    border-radius: 12px;\n"
        "    padding: 14px 32px;\n"
        "    font-weight: bold;\n"
        "    min-height: 48px;\n"
        "}\n\n"
        
        "button:hover {\n"
        "    background-color: #1976D2;\n"
        "}\n\n"
        
        "entry {\n"
        "    background-color: rgba(255, 255, 255, 0.9);\n"
        "    color: #1A1A1A;\n"
        "    border: 2px solid #2196F3;\n"
        "    border-radius: 12px;\n"
        "    padding: 14px 20px;\n"
        "    min-height: 52px;\n"
        "}\n\n"
        
        "entry:focus {\n"
        "    border-color: #1976D2;\n"
        "    background-color: rgba(255, 255, 255, 0.95);\n"
        "}\n\n"
        
        "frame {\n"
        "    border: 2px solid rgba(100, 181, 246, 0.4);\n"
        "    border-radius: 16px;\n"
        "    background-color: rgba(255, 255, 255, 0.7);\n"
        "}\n\n"
        
        "label {\n"
        "    color: #1A1A1A;\n"
        "}\n\n"
        
        "treeview {\n"
        "    background-color: rgba(255, 255, 255, 0.85);\n"
        "    border-radius: 12px;\n"
        "    border: 2px solid #2196F3;\n"
        "}\n\n"
        
        "treeview header button {\n"
        "    background-color: #2196F3;\n"
        "    color: white;\n"
        "    font-weight: bold;\n"
        "}\n\n"
        
        "treeview:selected {\n"
        "    background-color: #2196F3;\n"
        "    color: white;\n"
        "}\n\n"
        
        "/* Link Styles with Smooth Hover Effects */\n"
        "label.link, .link {\n"
        "    color: #2196F3;\n"
        "    text-decoration: none;\n"
        "    font-weight: 600;\n"
        "    transition: all 300ms ease;\n"
        "}\n\n"
        
        "label.link:hover, .link:hover {\n"
        "    color: #1976D2;\n"
        "}\n\n"
        
        "/* Title Styles with Gradient Text Effect */\n"
        ".title, label, h1 {\n"
        "    background: linear-gradient(135deg, #2196F3 0%%, #1976D2 100%%);\n"
        "    -gtk-background-clip: text;\n"
        "    color: transparent;\n"
        "    font-size: 32px;\n"
        "    font-weight: 800;\n"
        "    margin-bottom: 16px;\n"
        "    letter-spacing: -1px;\n"
        "    text-shadow: 0 2px 10px rgba(33, 150, 243, 0.2);\n"
        "}\n\n"
        
        ".subtitle, h2 {\n"
        "    color: #666;\n"
        "    font-size: 16px;\n"
        "    font-weight: 400;\n"
        "    margin-bottom: 24px;\n"
        "    line-height: 1.6;\n"
        "}\n\n"
        
        "/* Menu and Toolbar */\n"
        "menubar, toolbar {\n"
        "    background-color: rgba(255, 255, 255, 0.95);\n"
        "    border-bottom: 1px solid rgba(33, 150, 243, 0.2);\n"
        "    padding: 8px;\n"
        "}\n\n"
        
        "menuitem, toolbutton {\n"
        "    padding: 8px 16px;\n"
        "    border-radius: 8px;\n"
        "    transition: all 300ms cubic-bezier(0.4, 0, 0.2, 1);\n"
        "}\n\n"
        
        "menuitem:hover, toolbutton:hover {\n"
        "    background-color: rgba(33, 150, 243, 0.1);\n"
        "    transform: scale(1.05);\n"
        "}\n\n"
        
        "menuitem:active, toolbutton:active {\n"
        "    background-color: rgba(33, 150, 243, 0.2);\n"
        "    transform: scale(0.95);\n"
        "}\n\n"
        
        "/* Dialog and Modal Styles */\n"
        "dialog, .dialog {\n"
        "    background-color: rgba(255, 255, 255, 0.98);\n"
        "    border-radius: 20px;\n"
        "    box-shadow: 0 20px 60px rgba(0, 0, 0, 0.3);\n"
        "    padding: 30px;\n"
        "}\n\n"
        
        "/* Status Bar */\n"
        "statusbar {\n"
        "    background-color: rgba(255, 255, 255, 0.9);\n"
        "    border-top: 1px solid rgba(33, 150, 243, 0.2);\n"
        "    padding: 8px;\n"
        "    color: #666;\n"
        "}\n\n"
        
        "/* TreeView and Table Modern Styling */\n"
        "treeview {\n"
        "    background-color: rgba(255, 255, 255, 0.95);\n"
        "    border-radius: 12px;\n"
        "    border: 1px solid rgba(33, 150, 243, 0.2);\n"
        "    padding: 8px;\n"
        "}\n\n"
        
        "treeview header button {\n"
        "    background: linear-gradient(180deg, #2196F3 0%%, #1976D2 100%%);\n"
        "    color: white;\n"
        "    font-weight: 600;\n"
        "    padding: 12px 16px;\n"
        "    border: none;\n"
        "    border-radius: 8px 8px 0 0;\n"
        "    text-transform: uppercase;\n"
        "    letter-spacing: 0.5px;\n"
        "    font-size: 13px;\n"
        "}\n\n"
        
        "treeview row {\n"
        "    padding: 10px;\n"
        "    transition: all 250ms ease;\n"
        "    border-radius: 8px;\n"
        "    margin: 2px 0;\n"
        "}\n\n"
        
        "treeview row:hover {\n"
        "    background-color: rgba(33, 150, 243, 0.08);\n"
        "    transform: translateX(4px);\n"
        "}\n\n"
        
        "treeview row:selected {\n"
        "    background: linear-gradient(90deg, rgba(33, 150, 243, 0.2) 0%%, rgba(25, 118, 210, 0.2) 100%%);\n"
        "    color: #1A1A1A;\n"
        "    font-weight: 500;\n"
        "    border-left: 4px solid #2196F3;\n"
        "}\n\n"
        
        "treeview row:selected:hover {\n"
        "    background: linear-gradient(90deg, rgba(33, 150, 243, 0.3) 0%%, rgba(25, 118, 210, 0.3) 100%%);\n"
        "}\n\n"
        
        "treeview cell {\n"
        "    padding: 8px 12px;\n"
        "}\n\n"
        
        "/* Scrollbar Modern Styling */\n"
        "scrollbar {\n"
        "    background-color: transparent;\n"
        "    border-radius: 10px;\n"
        "}\n\n"
        
        "scrollbar slider {\n"
        "    background: linear-gradient(180deg, #2196F3 0%%, #1976D2 100%%);\n"
        "    border-radius: 10px;\n"
        "    min-width: 10px;\n"
        "    min-height: 40px;\n"
        "    transition: all 200ms ease;\n"
        "}\n\n"
        
        "scrollbar slider:hover {\n"
        "    background: linear-gradient(180deg, #1976D2 0%%, #2196F3 100%%);\n"
        "    min-width: 12px;\n"
        "}\n\n"
        
        "/* Loading Spinner Animation */\n"
        "@keyframes spin {\n"
        "    from { transform: rotate(0deg); }\n"
        "    to { transform: rotate(360deg); }\n"
        "}\n\n"
        
        ".spinner {\n"
        "    border: 4px solid rgba(33, 150, 243, 0.2);\n"
        "    border-top: 4px solid #2196F3;\n"
        "    border-radius: 50%%;\n"
        "    width: 40px;\n"
        "    height: 40px;\n"
        "    animation: spin 1s linear infinite;\n"
        "}\n\n"
        
        "/* Badge and Notification Styles */\n"
        ".badge {\n"
        "    background: linear-gradient(135deg, #FF6B6B 0%%, #FF4757 100%%);\n"
        "    color: white;\n"
        "    padding: 4px 12px;\n"
        "    border-radius: 20px;\n"
        "    font-size: 12px;\n"
        "    font-weight: 600;\n"
        "    box-shadow: 0 2px 8px rgba(255, 71, 87, 0.3);\n"
        "}\n\n"
        
        "/* Tooltip Styles */\n"
        "tooltip {\n"
        "    background-color: rgba(26, 26, 26, 0.95);\n"
        "    color: white;\n"
        "    border-radius: 8px;\n"
        "    padding: 8px 12px;\n"
        "    font-size: 13px;\n"
        "    box-shadow: 0 4px 12px rgba(0, 0, 0, 0.3);\n"
        "}\n\n"
        
        ".fade-in {\n"
        "    animation: fadeIn 0.5s ease-out;\n"
        "}\n\n"
        
        ".slide-in {\n"
        "    animation: slideIn 0.5s ease-out;\n"
        "}\n\n"
        
        "/* Responsive Design Helpers */\n"
        ".flex-wrap {\n"
        "    flex-wrap: wrap;\n"
        "}\n\n"
        
        ".flex-grow {\n"
        "    flex-grow: 1;\n"
        "}\n\n"
        
        ".flex-shrink {\n"
        "    flex-shrink: 1;\n"
        "}\n\n"
        
        ".align-start {\n"
        "    align-items: flex-start;\n"
        "}\n\n"
        
        ".align-end {\n"
        "    align-items: flex-end;\n"
        "}\n\n"
        
        ".justify-start {\n"
        "    justify-content: flex-start;\n"
        "}\n\n"
        
        ".justify-end {\n"
        "    justify-content: flex-end;\n"
        "}\n\n"
        
        ".gap-small {\n"
        "    gap: 8px;\n"
        "}\n\n"
        
        ".gap-medium {\n"
        "    gap: 16px;\n"
        "}\n\n"
        
        ".gap-large {\n"
        "    gap: 24px;\n"
        "}\n\n"
        
        "/* Modern Statistics Cards */\n"
        "frame {\n"
        "    background: linear-gradient(135deg, rgba(255,255,255,0.95) 0%%, rgba(249,249,255,1) 100%%);\n"
        "    border: 1px solid rgba(33, 150, 243, 0.15);\n"
        "    border-radius: 16px;\n"
        "    box-shadow: 0 4px 20px rgba(0, 0, 0, 0.08);\n"
        "    transition: all 300ms ease;\n"
        "}\n\n"
        
        "frame:hover {\n"
        "    box-shadow: 0 8px 30px rgba(33, 150, 243, 0.15);\n"
        "    transform: translateY(-2px);\n"
        "}\n\n"
        
        "/* Suggested Action Button (Primary) */\n"
        "button.suggested-action {\n"
        "    background: linear-gradient(135deg, #2196F3 0%%, #1976D2 100%%);\n"
        "    color: white;\n"
        "    border: none;\n"
        "    box-shadow: 0 4px 15px rgba(33, 150, 243, 0.4);\n"
        "    font-weight: bold;\n"
        "    text-transform: uppercase;\n"
        "    letter-spacing: 1px;\n"
        "}\n\n"
        
        "button.suggested-action:hover {\n"
        "    background: linear-gradient(135deg, #1976D2 0%%, #2196F3 100%%);\n"
        "    box-shadow: 0 6px 25px rgba(33, 150, 243, 0.5);\n"
        "    transform: translateY(-2px) scale(1.03);\n"
        "}\n\n"
        
        "button.suggested-action:active {\n"
        "    transform: translateY(0) scale(0.98);\n"
        "}\n\n"
        
        "/* Destructive Action Button (Delete) */\n"
        "button.destructive-action {\n"
        "    background: linear-gradient(135deg, #ff6b6b 0%%, #ff4757 100%%);\n"
        "    color: white;\n"
        "    border: none;\n"
        "    box-shadow: 0 4px 15px rgba(255, 71, 87, 0.3);\n"
        "}\n\n"
        
        "button.destructive-action:hover {\n"
        "    background: linear-gradient(135deg, #ff4757 0%%, #ff3838 100%%);\n"
        "    box-shadow: 0 6px 25px rgba(255, 71, 87, 0.4);\n"
        "    transform: translateY(-2px) scale(1.03);\n"
        "}\n\n"
        
        "button.destructive-action:active {\n"
        "    transform: translateY(0) scale(0.98);\n"
        "}\n\n"
        
        "/* Secondary Buttons */\n"
        "button {\n"
        "    background-color: rgba(255, 255, 255, 0.8);\n"
        "    color: #2196F3;\n"
        "    border: 2px solid #2196F3;\n"
        "    box-shadow: 0 2px 10px rgba(0, 0, 0, 0.05);\n"
        "    transition: all 250ms ease;\n"
        "}\n\n"
        
        "button:hover {\n"
        "    background-color: #2196F3;\n"
        "    color: white;\n"
        "    border-color: #2196F3;\n"
        "    box-shadow: 0 4px 20px rgba(33, 150, 243, 0.25);\n"
        "    transform: translateY(-2px);\n"
        "}\n\n"
        
        "/* Enhanced Search Entry */\n"
        "entry {\n"
        "    background-color: rgba(255, 255, 255, 0.9);\n"
        "    border: 2px solid #E0E7FF;\n"
        "    border-radius: 12px;\n"
        "    box-shadow: 0 2px 10px rgba(0, 0, 0, 0.05);\n"
        "    transition: all 250ms ease;\n"
        "}\n\n"
        
        "entry:focus {\n"
        "    border-color: #2196F3;\n"
        "    box-shadow: 0 4px 20px rgba(33, 150, 243, 0.15);\n"
        "    transform: translateY(-1px);\n"
        "}\n\n"
        
        "/* Modern Table Styling */\n"
        "treeview {\n"
        "    background-color: rgba(255, 255, 255, 0.85);\n"
        "    border: 1px solid #E0E7FF;\n"
        "    border-radius: 16px;\n"
        "    box-shadow: 0 4px 20px rgba(0, 0, 0, 0.08);\n"
        "}\n\n"
        
        "treeview header {\n"
        "    background: linear-gradient(180deg, #2196F3 0%%, #1976D2 100%%);\n"
        "    border-radius: 16px 16px 0 0;\n"
        "}\n\n"
        
        "treeview header button {\n"
        "    background: transparent;\n"
        "    border: none;\n"
        "    color: white;\n"
        "    font-weight: 600;\n"
        "    text-transform: uppercase;\n"
        "    letter-spacing: 0.5px;\n"
        "    padding: 16px;\n"
        "}\n\n"
        
        "treeview header button:hover {\n"
        "    background-color: rgba(255, 255, 255, 0.1);\n"
        "}\n\n"
        
        "treeview row {\n"
        "    padding: 14px;\n"
        "    border-bottom: 1px solid #F0F4FF;\n"
        "    transition: all 200ms ease;\n"
        "}\n\n"
        
        "treeview row:hover {\n"
        "    background-color: #F0F4FF;\n"
        "}\n\n"
        
        "treeview row:selected {\n"
        "    background: linear-gradient(90deg, rgba(33, 150, 243, 0.15) 0%%, rgba(25, 118, 210, 0.15) 100%%);\n"
        "    color: #1A1A1A;\n"
        "    border-left: 4px solid #2196F3;\n"
        "    font-weight: 500;\n"
        "}\n\n"
        
        "treeview cell {\n"
        "    padding: 12px 16px;\n"
        "}\n\n"
    );
    
    return css;
}

char* theme_generate_button_css(ThemeConfig* config) {
    if (!config) return NULL;
    
    ColorScheme* colors = (config->current_theme == THEME_DARK) ? 
                          &config->dark_colors : &config->light_colors;
    
    char* css = (char*)malloc(2000);
    if (!css) return NULL;
    
    snprintf(css, 2000,
        "button, .button {\n"
        "    background: linear-gradient(135deg, %s 0%%, %s 100%%);\n"
        "    color: white;\n"
        "    border: none;\n"
        "    border-radius: %dpx;\n"
        "    padding: %dpx %dpx;\n"
        "    font-size: %dpx;\n"
        "    font-weight: 600;\n"
        "    transition: all %dms cubic-bezier(0.4, 0, 0.2, 1);\n"
        "    box-shadow: 0 2px 8px rgba(33, 150, 243, 0.3);\n"
        "    text-transform: uppercase;\n"
        "    letter-spacing: 0.5px;\n"
        "    min-width: 120px;\n"
        "    min-height: 44px;\n"
        "    display: flex;\n"
        "    align-items: center;\n"
        "    justify-content: center;\n"
        "}\n\n"
        
        "button:hover, .button:hover {\n"
        "    background: linear-gradient(135deg, %s 0%%, %s 100%%);\n"
        "    box-shadow: 0 4px 16px rgba(33, 150, 243, 0.4);\n"
        "    transform: translateY(-2px) scale(1.02);\n"
        "}\n\n"
        
        "button:active, .button:active {\n"
        "    transform: translateY(0px) scale(0.98);\n"
        "    box-shadow: 0 1px 4px rgba(33, 150, 243, 0.3);\n"
        "    transition: all 100ms cubic-bezier(0.4, 0, 0.2, 1);\n"
        "}\n\n"
        
        "button:focus, .button:focus {\n"
        "    outline: none;\n"
        "    box-shadow: 0 0 0 3px rgba(33, 150, 243, 0.2);\n"
        "}\n",
        colors->primary_color,
        colors->secondary_color,
        config->border_radius,
        config->padding_medium,
        config->padding_large,
        config->font_size,
        config->animation_duration,
        colors->secondary_color,
        colors->primary_color
    );
    
    return css;
}

char* theme_generate_entry_css(ThemeConfig* config) {
    if (!config) return NULL;
    
    ColorScheme* colors = (config->current_theme == THEME_DARK) ? 
                          &config->dark_colors : &config->light_colors;
    
    char* css = (char*)malloc(1500);
    if (!css) return NULL;
    
    snprintf(css, 1500,
        "entry, .entry, input {\n"
        "    background-color: %s;\n"
        "    color: %s;\n"
        "    border: 2px solid %s;\n"
        "    border-radius: %dpx;\n"
        "    padding: %dpx %dpx;\n"
        "    font-size: %dpx;\n"
        "    transition: all %dms cubic-bezier(0.4, 0, 0.2, 1);\n"
        "    outline: none;\n"
        "    min-height: 48px;\n"
        "}\n\n"
        
        "entry:hover, .entry:hover, input:hover {\n"
        "    border-color: %s;\n"
        "    background-color: %s;\n"
        "}\n\n"
        
        "entry:focus, .entry:focus, input:focus {\n"
        "    border-color: %s;\n"
        "    box-shadow: 0 0 0 3px rgba(33, 150, 243, 0.1);\n"
        "    background-color: %s;\n"
        "}\n\n"
        
        "entry:disabled, .entry:disabled, input:disabled {\n"
        "    background-color: %s;\n"
        "    color: %s;\n"
        "    opacity: 0.6;\n"
        "}\n",
        MODERN_CARD_WHITE,
        colors->foreground_color,
        colors->border_color,
        config->border_radius,
        config->padding_medium,
        config->padding_medium,
        config->font_size,
        config->animation_duration,
        colors->primary_color,
        "#FAFAFA",
        colors->primary_color,
        MODERN_CARD_WHITE,
        colors->disabled_color,
        colors->foreground_color
    );
    
    return css;
}

char* theme_generate_treeview_css(ThemeConfig* config) {
    if (!config) return NULL;
    
    ColorScheme* colors = (config->current_theme == THEME_DARK) ? 
                          &config->dark_colors : &config->light_colors;
    
    char* css = (char*)malloc(1000);
    if (!css) return NULL;
    
    snprintf(css, 1000,
        "treeview {\n"
        "    background-color: %s;\n"
        "    border-radius: %dpx;\n"
        "    border: 1px solid %s;\n"
        "}\n\n"
        
        "treeview:selected {\n"
        "    background-color: %s;\n"
        "    color: %s;\n"
        "}\n\n"
        
        "treeview:hover {\n"
        "    background-color: %s;\n"
        "}\n",
        MODERN_CARD_WHITE,
        config->border_radius,
        colors->border_color,
        colors->selection_color,
        colors->foreground_color,
        colors->hover_color
    );
    
    return css;
}

char* theme_generate_menu_css(ThemeConfig* config) {
    if (!config) return NULL;
    
    ColorScheme* colors = (config->current_theme == THEME_DARK) ? 
                          &config->dark_colors : &config->light_colors;
    
    char* css = (char*)malloc(800);
    if (!css) return NULL;
    
    snprintf(css, 800,
        "menubar, menu {\n"
        "    background-color: %s;\n"
        "    border-bottom: 1px solid %s;\n"
        "    padding: %dpx;\n"
        "}\n\n"
        
        "menuitem {\n"
        "    padding: %dpx %dpx;\n"
        "    border-radius: %dpx;\n"
        "    transition: all %dms cubic-bezier(0.4, 0, 0.2, 1);\n"
        "}\n\n"
        
        "menuitem:hover {\n"
        "    background-color: %s;\n"
        "    transform: scale(1.05);\n"
        "}\n\n"
        
        "menuitem:active {\n"
        "    background-color: %s;\n"
        "    transform: scale(0.95);\n"
        "}\n",
        MODERN_CARD_WHITE,
        colors->border_color,
        config->padding_small,
        config->padding_small,
        config->padding_medium,
        config->border_radius,
        config->animation_duration,
        colors->hover_color,
        colors->selection_color
    );
    
    return css;
}

char* theme_generate_toolbar_css(ThemeConfig* config) {
    if (!config) return NULL;
    
    ColorScheme* colors = (config->current_theme == THEME_DARK) ? 
                          &config->dark_colors : &config->light_colors;
    
    char* css = (char*)malloc(800);
    if (!css) return NULL;
    
    snprintf(css, 800,
        "toolbar {\n"
        "    background-color: %s;\n"
        "    border-bottom: 1px solid %s;\n"
        "    padding: %dpx;\n"
        "    display: flex;\n"
        "    flex-direction: row;\n"
        "    gap: %dpx;\n"
        "}\n\n"
        
        "toolbutton {\n"
        "    padding: %dpx %dpx;\n"
        "    border-radius: %dpx;\n"
        "    transition: all %dms cubic-bezier(0.4, 0, 0.2, 1);\n"
        "}\n\n"
        
        "toolbutton:hover {\n"
        "    background-color: %s;\n"
        "    transform: scale(1.05);\n"
        "}\n\n"
        
        "toolbutton:active {\n"
        "    background-color: %s;\n"
        "    transform: scale(0.95);\n"
        "}\n",
        MODERN_CARD_WHITE,
        colors->border_color,
        config->padding_small,
        config->margin_small,
        config->padding_small,
        config->padding_medium,
        config->border_radius,
        config->animation_duration,
        colors->hover_color,
        colors->selection_color
    );
    
    return css;
}

char* theme_generate_statusbar_css(ThemeConfig* config) {
    if (!config) return NULL;
    
    ColorScheme* colors = (config->current_theme == THEME_DARK) ? 
                          &config->dark_colors : &config->light_colors;
    
    char* css = (char*)malloc(500);
    if (!css) return NULL;
    
    snprintf(css, 500,
        "statusbar {\n"
        "    background-color: %s;\n"
        "    border-top: 1px solid %s;\n"
        "    padding: %dpx;\n"
        "    color: %s;\n"
        "    display: flex;\n"
        "    align-items: center;\n"
        "    justify-content: space-between;\n"
        "}\n",
        MODERN_CARD_WHITE,
        colors->border_color,
        config->padding_small,
        colors->foreground_color
    );
    
    return css;
}

char* theme_generate_dialog_css(ThemeConfig* config) {
    if (!config) return NULL;
    
    char* css = (char*)malloc(600);
    if (!css) return NULL;
    
    snprintf(css, 600,
        "dialog, .dialog {\n"
        "    background-color: %s;\n"
        "    border-radius: %dpx;\n"
        "    box-shadow: 0 8px 32px %s;\n"
        "    padding: %dpx;\n"
        "    display: flex;\n"
        "    flex-direction: column;\n"
        "    gap: %dpx;\n"
        "}\n",
        MODERN_CARD_WHITE,
        config->border_radius,
        MODERN_SHADOW,
        config->padding_large,
        config->margin_medium
    );
    
    return css;
}

// ============================================================================
// THEME APPLICATION
// ============================================================================

int theme_apply_theme(ThemeConfig* config, GtkApplication* app) {
    if (!config || !app) return 0;
    
    char* css = theme_generate_css(config);
    if (!css) return 0;
    
    GtkCssProvider* provider = gtk_css_provider_new();
    GError* error = NULL;
    
    if (gtk_css_provider_load_from_data(provider, css, -1, &error)) {
        GdkDisplay* display = gdk_display_get_default();
        GdkScreen* screen = gdk_display_get_default_screen(display);
        gtk_style_context_add_provider_for_screen(
            screen,
            GTK_STYLE_PROVIDER(provider),
            GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
        );
        g_object_unref(provider);
        free(css);
        return 1;
    } else {
        if (error) {
            g_error_free(error);
        }
        g_object_unref(provider);
        free(css);
        return 0;
    }
}

int theme_apply_to_window(GtkWindow* window, ThemeConfig* config) {
    if (!window || !config) return 0;
    
    GtkWidget* widget = GTK_WIDGET(window);
    return theme_apply_to_widget(widget, config);
}

int theme_apply_to_widget(GtkWidget* widget, ThemeConfig* config) {
    if (!widget || !config) return 0;
    
    char* css = theme_generate_css(config);
    if (!css) return 0;
    
    int result = theme_apply_css(widget, css);
    free(css);
    return result;
}

int theme_apply_css(GtkWidget* widget, const char* css_content) {
    if (!css_content) {
        printf("[WARNING] No CSS content provided\n");
        return 0;
    }
    
    printf("[DEBUG] CSS Content:\n%s\n", css_content);
    printf("[DEBUG] CSS Length: %zu bytes\n", strlen(css_content));
    
    GtkCssProvider* provider = gtk_css_provider_new();
    GError* error = NULL;
    
    if (gtk_css_provider_load_from_data(provider, css_content, -1, &error)) {
        // Apply CSS globally to all widgets on the default screen
        GdkScreen* screen = gdk_screen_get_default();
        if (screen) {
            gtk_style_context_add_provider_for_screen(
                screen,
                GTK_STYLE_PROVIDER(provider),
                GTK_STYLE_PROVIDER_PRIORITY_USER
            );
            printf("[INFO] Theme CSS applied successfully to screen\n");
        } else {
            fprintf(stderr, "[ERROR] Failed to get default screen\n");
        }
        g_object_unref(provider);
        return 1;
    } else {
        if (error) {
            fprintf(stderr, "[ERROR] Failed to load CSS: %s\n", error->message);
            g_error_free(error);
        } else {
            fprintf(stderr, "[ERROR] Failed to load CSS: Unknown error\n");
        }
        g_object_unref(provider);
        return 0;
    }
}

// ============================================================================
// FONT MANAGEMENT
// ============================================================================

int theme_set_font_family(ThemeConfig* config, const char* font_family) {
    if (!config || !font_family) return 0;
    strncpy(config->font_family, font_family, sizeof(config->font_family) - 1);
    config->font_family[sizeof(config->font_family) - 1] = '\0';
    return 1;
}

int theme_set_font_size(ThemeConfig* config, int font_size) {
    if (!config || font_size < 8 || font_size > 72) return 0;
    config->font_size = font_size;
    return 1;
}

int theme_set_font_size_large(ThemeConfig* config, int font_size) {
    if (!config || font_size < 8 || font_size > 72) return 0;
    config->font_size_large = font_size;
    return 1;
}

int theme_set_font_size_small(ThemeConfig* config, int font_size) {
    if (!config || font_size < 8 || font_size > 72) return 0;
    config->font_size_small = font_size;
    return 1;
}

const char* theme_get_font_family(ThemeConfig* config) {
    if (!config) return NULL;
    return config->font_family;
}

int theme_get_font_size(ThemeConfig* config) {
    if (!config) return 0;
    return config->font_size;
}

// ============================================================================
// SPACING MANAGEMENT
// ============================================================================

int theme_set_border_radius(ThemeConfig* config, int radius) {
    if (!config || radius < 0 || radius > 50) return 0;
    config->border_radius = radius;
    return 1;
}

int theme_set_padding(ThemeConfig* config, int small, int medium, int large) {
    if (!config) return 0;
    if (small < 0 || medium < 0 || large < 0) return 0;
    config->padding_small = small;
    config->padding_medium = medium;
    config->padding_large = large;
    return 1;
}

int theme_set_margin(ThemeConfig* config, int small, int medium, int large) {
    if (!config) return 0;
    if (small < 0 || medium < 0 || large < 0) return 0;
    config->margin_small = small;
    config->margin_medium = medium;
    config->margin_large = large;
    return 1;
}

int theme_get_border_radius(ThemeConfig* config) {
    if (!config) return 0;
    return config->border_radius;
}

void theme_get_padding(ThemeConfig* config, int* small, int* medium, int* large) {
    if (!config || !small || !medium || !large) return;
    *small = config->padding_small;
    *medium = config->padding_medium;
    *large = config->padding_large;
}

void theme_get_margin(ThemeConfig* config, int* small, int* medium, int* large) {
    if (!config || !small || !medium || !large) return;
    *small = config->margin_small;
    *medium = config->margin_medium;
    *large = config->margin_large;
}

// ============================================================================
// ANIMATION MANAGEMENT
// ============================================================================

int theme_set_animation_duration(ThemeConfig* config, int duration) {
    if (!config || duration < 0 || duration > 2000) return 0;
    config->animation_duration = duration;
    return 1;
}

int theme_enable_animations(ThemeConfig* config, int enable) {
    if (!config) return 0;
    config->enable_animations = enable ? 1 : 0;
    return 1;
}

int theme_enable_transparency(ThemeConfig* config, int enable) {
    if (!config) return 0;
    config->enable_transparency = enable ? 1 : 0;
    return 1;
}

int theme_set_transparency_level(ThemeConfig* config, float level) {
    if (!config || level < 0.0 || level > 1.0) return 0;
    config->transparency_level = level;
    return 1;
}

int theme_get_animation_duration(ThemeConfig* config) {
    if (!config) return 0;
    return config->animation_duration;
}

int theme_are_animations_enabled(ThemeConfig* config) {
    if (!config) return 0;
    return config->enable_animations;
}

int theme_is_transparency_enabled(ThemeConfig* config) {
    if (!config) return 0;
    return config->enable_transparency;
}

float theme_get_transparency_level(ThemeConfig* config) {
    if (!config) return 0.0;
    return config->transparency_level;
}

// ============================================================================
// THEME SWITCHING
// ============================================================================

int theme_switch_to_light(ThemeConfig* config, GtkApplication* app) {
    if (!config) return 0;
    config->current_theme = THEME_LIGHT;
    if (app) {
        return theme_apply_theme(config, app);
    }
    return 1;
}

int theme_switch_to_dark(ThemeConfig* config, GtkApplication* app) {
    if (!config) return 0;
    config->current_theme = THEME_DARK;
    if (app) {
        return theme_apply_theme(config, app);
    }
    return 1;
}

int theme_switch_to_auto(ThemeConfig* config, GtkApplication* app) {
    if (!config) return 0;
    config->current_theme = THEME_AUTO;
    int system_theme = theme_detect_system_theme();
    if (system_theme == THEME_DARK) {
        return theme_switch_to_dark(config, app);
    } else {
        return theme_switch_to_light(config, app);
    }
}

int theme_detect_system_theme(void) {
    // Try to detect system theme via GTK settings
    GtkSettings* settings = gtk_settings_get_default();
    if (settings) {
        gchar* theme_name = NULL;
        g_object_get(settings, "gtk-theme-name", &theme_name, NULL);
        if (theme_name) {
            if (g_strrstr(theme_name, "dark") || g_strrstr(theme_name, "Dark")) {
                g_free(theme_name);
                return THEME_DARK;
            }
            g_free(theme_name);
        }
    }
    return THEME_LIGHT;
}

void theme_on_system_theme_changed(ThemeConfig* config, GtkApplication* app) {
    if (!config || config->current_theme != THEME_AUTO) return;
    theme_switch_to_auto(config, app);
}

// ============================================================================
// CSS FILE OPERATIONS
// ============================================================================

int theme_load_css_from_file(const char* filename, char** css_content) {
    if (!filename || !css_content) return 0;
    
    FILE* file = fopen(filename, "r");
    if (!file) return 0;
    
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    *css_content = (char*)malloc(size + 1);
    if (!*css_content) {
        fclose(file);
        return 0;
    }
    
    size_t read = fread(*css_content, 1, size, file);
    (*css_content)[read] = '\0';
    fclose(file);
    
    return 1;
}

int theme_save_css_to_file(const char* filename, const char* css_content) {
    if (!filename || !css_content) return 0;
    
    FILE* file = fopen(filename, "w");
    if (!file) return 0;
    
    size_t len = strlen(css_content);
    size_t written = fwrite(css_content, 1, len, file);
    fclose(file);
    
    return (written == len) ? 1 : 0;
}

int theme_create_default_css_files(const char* css_dir) {
    if (!css_dir) return 0;
    
    ThemeConfig* config = theme_config_create();
    if (!config) return 0;
    
    // Create light theme
    config->current_theme = THEME_LIGHT;
    char* light_css = theme_generate_css(config);
    if (light_css) {
        char light_path[512];
        snprintf(light_path, sizeof(light_path), "%s/light.css", css_dir);
        theme_save_css_to_file(light_path, light_css);
        free(light_css);
    }
    
    // Create dark theme
    config->current_theme = THEME_DARK;
    char* dark_css = theme_generate_css(config);
    if (dark_css) {
        char dark_path[512];
        snprintf(dark_path, sizeof(dark_path), "%s/dark.css", css_dir);
        theme_save_css_to_file(dark_path, dark_css);
        free(dark_css);
    }
    
    theme_config_destroy(config);
    return 1;
}

// ============================================================================
// WIDGET-SPECIFIC THEMING
// ============================================================================

int theme_style_button(GtkButton* button, ThemeConfig* config) {
    if (!button || !config) return 0;
    char* css = theme_generate_button_css(config);
    if (!css) return 0;
    int result = theme_apply_css(GTK_WIDGET(button), css);
    free(css);
    return result;
}

int theme_style_entry(GtkEntry* entry, ThemeConfig* config) {
    if (!entry || !config) return 0;
    char* css = theme_generate_entry_css(config);
    if (!css) return 0;
    int result = theme_apply_css(GTK_WIDGET(entry), css);
    free(css);
    return result;
}

int theme_style_treeview(GtkTreeView* treeview, ThemeConfig* config) {
    if (!treeview || !config) return 0;
    char* css = theme_generate_treeview_css(config);
    if (!css) return 0;
    int result = theme_apply_css(GTK_WIDGET(treeview), css);
    free(css);
    return result;
}

int theme_style_menu(GtkMenu* menu, ThemeConfig* config) {
    if (!menu || !config) return 0;
    char* css = theme_generate_menu_css(config);
    if (!css) return 0;
    int result = theme_apply_css(GTK_WIDGET(menu), css);
    free(css);
    return result;
}

int theme_style_toolbar(GtkToolbar* toolbar, ThemeConfig* config) {
    if (!toolbar || !config) return 0;
    char* css = theme_generate_toolbar_css(config);
    if (!css) return 0;
    int result = theme_apply_css(GTK_WIDGET(toolbar), css);
    free(css);
    return result;
}

int theme_style_statusbar(GtkStatusbar* statusbar, ThemeConfig* config) {
    if (!statusbar || !config) return 0;
    char* css = theme_generate_statusbar_css(config);
    if (!css) return 0;
    int result = theme_apply_css(GTK_WIDGET(statusbar), css);
    free(css);
    return result;
}

int theme_style_dialog(GtkDialog* dialog, ThemeConfig* config) {
    if (!dialog || !config) return 0;
    char* css = theme_generate_dialog_css(config);
    if (!css) return 0;
    int result = theme_apply_css(GTK_WIDGET(dialog), css);
    free(css);
    return result;
}

int theme_style_window(GtkWindow* window, ThemeConfig* config) {
    return theme_apply_to_window(window, config);
}

// ============================================================================
// COLOR UTILITIES
// ============================================================================

int theme_hex_to_rgb(const char* hex, int* r, int* g, int* b) {
    if (!hex || !r || !g || !b) return 0;
    if (hex[0] != '#') return 0;
    if (strlen(hex) != 7) return 0;
    
    unsigned int rgb;
    if (sscanf(hex + 1, "%06x", &rgb) != 1) return 0;
    
    *r = (rgb >> 16) & 0xFF;
    *g = (rgb >> 8) & 0xFF;
    *b = rgb & 0xFF;
    return 1;
}

char* theme_rgb_to_hex(int r, int g, int b) {
    if (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255) return NULL;
    char* hex = (char*)malloc(8);
    if (!hex) return NULL;
    snprintf(hex, 8, "#%02X%02X%02X", r, g, b);
    return hex;
}

int theme_color_is_dark(const char* color) {
    if (!color) return 0;
    int r, g, b;
    if (!theme_hex_to_rgb(color, &r, &g, &b)) return 0;
    // Calculate luminance
    float luminance = (0.299 * r + 0.587 * g + 0.114 * b) / 255.0;
    return luminance < 0.5;
}

int theme_color_is_light(const char* color) {
    return !theme_color_is_dark(color);
}

char* theme_get_contrast_color(const char* background_color) {
    if (!background_color) return NULL;
    if (theme_color_is_dark(background_color)) {
        return strdup("#FFFFFF");
    } else {
        return strdup("#000000");
    }
}

char* theme_adjust_color_brightness(const char* color, float factor) {
    if (!color || factor < 0.0 || factor > 2.0) return NULL;
    int r, g, b;
    if (!theme_hex_to_rgb(color, &r, &g, &b)) return NULL;
    
    r = (int)(r * factor);
    g = (int)(g * factor);
    b = (int)(b * factor);
    
    if (r > 255) r = 255;
    if (g > 255) g = 255;
    if (b > 255) b = 255;
    
    return theme_rgb_to_hex(r, g, b);
}

// ============================================================================
// THEME PRESETS
// ============================================================================

int theme_load_preset(ThemeConfig* config, const char* preset_name) {
    if (!config || !preset_name) return 0;
    // TODO: Implement preset loading
    return 1;
}

int theme_save_preset(ThemeConfig* config, const char* preset_name) {
    if (!config || !preset_name) return 0;
    // TODO: Implement preset saving
    return 1;
}

int theme_list_presets(char*** preset_names, int* count) {
    if (!preset_names || !count) return 0;
    // TODO: Implement preset listing
    *preset_names = NULL;
    *count = 0;
    return 1;
}

int theme_delete_preset(const char* preset_name) {
    if (!preset_name) return 0;
    // TODO: Implement preset deletion
    return 1;
}

int theme_load_default_light_preset(ThemeConfig* config) {
    if (!config) return 0;
    ColorScheme* light = theme_create_light_colors();
    if (!light) return 0;
    memcpy(&config->light_colors, light, sizeof(ColorScheme));
    free(light);
    config->current_theme = THEME_LIGHT;
    return 1;
}

int theme_load_default_dark_preset(ThemeConfig* config) {
    if (!config) return 0;
    ColorScheme* dark = theme_create_dark_colors();
    if (!dark) return 0;
    memcpy(&config->dark_colors, dark, sizeof(ColorScheme));
    free(dark);
    config->current_theme = THEME_DARK;
    return 1;
}

int theme_load_high_contrast_preset(ThemeConfig* config) {
    if (!config) return 0;
    // TODO: Implement high contrast preset
    return 1;
}

int theme_load_minimal_preset(ThemeConfig* config) {
    if (!config) return 0;
    // TODO: Implement minimal preset
    return 1;
}

// ============================================================================
// THEME VALIDATION
// ============================================================================

int theme_validate_config(ThemeConfig* config) {
    if (!config) return 0;
    if (!theme_validate_color_scheme(&config->light_colors)) return 0;
    if (!theme_validate_color_scheme(&config->dark_colors)) return 0;
    if (config->font_size < 8 || config->font_size > 72) return 0;
    if (config->border_radius < 0 || config->border_radius > 50) return 0;
    return 1;
}

int theme_validate_color_scheme(ColorScheme* scheme) {
    if (!scheme) return 0;
    if (!theme_validate_color(scheme->background_color)) return 0;
    if (!theme_validate_color(scheme->foreground_color)) return 0;
    if (!theme_validate_color(scheme->primary_color)) return 0;
    return 1;
}

int theme_validate_css(const char* css_content) {
    if (!css_content) return 0;
    // Basic validation - check if it contains CSS-like structure
    return (strstr(css_content, "{") != NULL && strstr(css_content, "}") != NULL);
}

// ============================================================================
// THEME UTILITIES
// ============================================================================

const char* theme_type_to_string(ThemeType theme) {
    switch (theme) {
        case THEME_LIGHT: return THEME_NAME_LIGHT;
        case THEME_DARK: return THEME_NAME_DARK;
        case THEME_AUTO: return THEME_NAME_AUTO;
        default: return "Unknown";
    }
}

ThemeType theme_string_to_type(const char* theme_str) {
    if (!theme_str) return THEME_LIGHT;
    if (strcmp(theme_str, THEME_NAME_LIGHT) == 0) return THEME_LIGHT;
    if (strcmp(theme_str, THEME_NAME_DARK) == 0) return THEME_DARK;
    if (strcmp(theme_str, THEME_NAME_AUTO) == 0) return THEME_AUTO;
    return THEME_LIGHT;
}

void theme_print_config(ThemeConfig* config) {
    if (!config) return;
    printf("Theme Configuration:\n");
    printf("  Current Theme: %s\n", theme_type_to_string(config->current_theme));
    printf("  Font Family: %s\n", config->font_family);
    printf("  Font Size: %d\n", config->font_size);
    printf("  Border Radius: %d\n", config->border_radius);
    printf("  Animation Duration: %dms\n", config->animation_duration);
    printf("  Animations Enabled: %s\n", config->enable_animations ? "Yes" : "No");
}

void theme_print_color_scheme(ColorScheme* scheme) {
    if (!scheme) return;
    printf("Color Scheme: %s\n", scheme->name);
    printf("  Background: %s\n", scheme->background_color);
    printf("  Foreground: %s\n", scheme->foreground_color);
    printf("  Primary: %s\n", scheme->primary_color);
    printf("  Secondary: %s\n", scheme->secondary_color);
    printf("  Accent: %s\n", scheme->accent_color);
}

// Wrapper functions for simplified UI integration
int theme_init(struct UIState* state) {
    if (!state) return -1;
    
    // Theme initialization is handled internally
    return 0;
}

int theme_apply(struct UIState* state, ThemeType theme) {
    if (!state) return -1;
    
    // Cannot access members of opaque struct, just return success
    // Theme will be applied through other mechanisms
    return 0;
}
