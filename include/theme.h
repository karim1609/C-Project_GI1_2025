#ifndef THEME_H
#define THEME_H

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"

// Forward declarations to avoid circular dependency
struct UIState;

// Theme types
typedef enum {
    THEME_LIGHT = 0,
    THEME_DARK = 1,
    THEME_AUTO = 2
} ThemeType;

// Color scheme structure
typedef struct {
    char name[50];
    char background_color[20];
    char foreground_color[20];
    char primary_color[20];
    char secondary_color[20];
    char accent_color[20];
    char error_color[20];
    char warning_color[20];
    char success_color[20];
    char info_color[20];
    char border_color[20];
    char selection_color[20];
    char hover_color[20];
    char disabled_color[20];
} ColorScheme;

// Theme configuration
typedef struct {
    ThemeType current_theme;
    ColorScheme light_colors;
    ColorScheme dark_colors;
    char font_family[50];
    int font_size;
    int font_size_large;
    int font_size_small;
    int border_radius;
    int padding_small;
    int padding_medium;
    int padding_large;
    int margin_small;
    int margin_medium;
    int margin_large;
    int animation_duration;
    int enable_animations;
    int enable_transparency;
    float transparency_level;
} ThemeConfig;

// Theme management functions
ThemeConfig* theme_config_create(void);
void theme_config_destroy(ThemeConfig* config);
int theme_config_load(ThemeConfig* config, const char* config_file);
int theme_config_save(ThemeConfig* config, const char* config_file);
int theme_config_set_theme(ThemeConfig* config, ThemeType theme);

// Theme application
int theme_apply_theme(ThemeConfig* config, GtkApplication* app);
int theme_apply_to_window(GtkWindow* window, ThemeConfig* config);
int theme_apply_to_widget(GtkWidget* widget, ThemeConfig* config);
int theme_apply_css(GtkWidget* widget, const char* css_content);

// CSS generation
char* theme_generate_css(ThemeConfig* config);
char* theme_generate_button_css(ThemeConfig* config);
char* theme_generate_entry_css(ThemeConfig* config);
char* theme_generate_treeview_css(ThemeConfig* config);
char* theme_generate_menu_css(ThemeConfig* config);
char* theme_generate_toolbar_css(ThemeConfig* config);
char* theme_generate_statusbar_css(ThemeConfig* config);
char* theme_generate_dialog_css(ThemeConfig* config);

// Color management
ColorScheme* theme_create_light_colors(void);
ColorScheme* theme_create_dark_colors(void);
ColorScheme* theme_create_custom_colors(const char* name);
int theme_set_color(ColorScheme* scheme, const char* color_name, const char* color_value);
const char* theme_get_color(ColorScheme* scheme, const char* color_name);
int theme_validate_color(const char* color);

// Font management
int theme_set_font_family(ThemeConfig* config, const char* font_family);
int theme_set_font_size(ThemeConfig* config, int font_size);

// Simplified theme interface
int theme_init(struct UIState* state);
int theme_apply(struct UIState* state, ThemeType theme);
int theme_set_font_size_large(ThemeConfig* config, int font_size);
int theme_set_font_size_small(ThemeConfig* config, int font_size);
const char* theme_get_font_family(ThemeConfig* config);
int theme_get_font_size(ThemeConfig* config);

// Spacing management
int theme_set_border_radius(ThemeConfig* config, int radius);
int theme_set_padding(ThemeConfig* config, int small, int medium, int large);
int theme_set_margin(ThemeConfig* config, int small, int medium, int large);
int theme_get_border_radius(ThemeConfig* config);
void theme_get_padding(ThemeConfig* config, int* small, int* medium, int* large);
void theme_get_margin(ThemeConfig* config, int* small, int* medium, int* large);

// Animation management
int theme_set_animation_duration(ThemeConfig* config, int duration);
int theme_enable_animations(ThemeConfig* config, int enable);
int theme_enable_transparency(ThemeConfig* config, int enable);
int theme_set_transparency_level(ThemeConfig* config, float level);
int theme_get_animation_duration(ThemeConfig* config);
int theme_are_animations_enabled(ThemeConfig* config);
int theme_is_transparency_enabled(ThemeConfig* config);
float theme_get_transparency_level(ThemeConfig* config);

// Theme switching
int theme_switch_to_light(ThemeConfig* config, GtkApplication* app);
int theme_switch_to_dark(ThemeConfig* config, GtkApplication* app);
int theme_switch_to_auto(ThemeConfig* config, GtkApplication* app);
int theme_detect_system_theme(void);
void theme_on_system_theme_changed(ThemeConfig* config, GtkApplication* app);

// CSS file operations
int theme_load_css_from_file(const char* filename, char** css_content);
int theme_save_css_to_file(const char* filename, const char* css_content);
int theme_create_default_css_files(const char* css_dir);

// Widget-specific theming
int theme_style_button(GtkButton* button, ThemeConfig* config);
int theme_style_entry(GtkEntry* entry, ThemeConfig* config);
int theme_style_treeview(GtkTreeView* treeview, ThemeConfig* config);
int theme_style_menu(GtkMenu* menu, ThemeConfig* config);
int theme_style_toolbar(GtkToolbar* toolbar, ThemeConfig* config);
int theme_style_statusbar(GtkStatusbar* statusbar, ThemeConfig* config);
int theme_style_dialog(GtkDialog* dialog, ThemeConfig* config);
int theme_style_window(GtkWindow* window, ThemeConfig* config);

// Color utilities
int theme_hex_to_rgb(const char* hex, int* r, int* g, int* b);
char* theme_rgb_to_hex(int r, int g, int b);
int theme_color_is_dark(const char* color);
int theme_color_is_light(const char* color);
char* theme_get_contrast_color(const char* background_color);
char* theme_adjust_color_brightness(const char* color, float factor);

// Theme presets
int theme_load_preset(ThemeConfig* config, const char* preset_name);
int theme_save_preset(ThemeConfig* config, const char* preset_name);
int theme_list_presets(char*** preset_names, int* count);
int theme_delete_preset(const char* preset_name);

// Default presets
int theme_load_default_light_preset(ThemeConfig* config);
int theme_load_default_dark_preset(ThemeConfig* config);
int theme_load_high_contrast_preset(ThemeConfig* config);
int theme_load_minimal_preset(ThemeConfig* config);

// Theme validation
int theme_validate_config(ThemeConfig* config);
int theme_validate_color_scheme(ColorScheme* scheme);
int theme_validate_css(const char* css_content);

// Theme utilities
const char* theme_type_to_string(ThemeType theme);
ThemeType theme_string_to_type(const char* theme_str);
void theme_print_config(ThemeConfig* config);
void theme_print_color_scheme(ColorScheme* scheme);

// Constants
#define THEME_DEFAULT_FONT_FAMILY "Roboto"
#define THEME_DEFAULT_FONT_SIZE 12
#define THEME_DEFAULT_FONT_SIZE_LARGE 16
#define THEME_DEFAULT_FONT_SIZE_SMALL 10
#define THEME_DEFAULT_BORDER_RADIUS 4
#define THEME_DEFAULT_PADDING_SMALL 4
#define THEME_DEFAULT_PADDING_MEDIUM 8
#define THEME_DEFAULT_PADDING_LARGE 16
#define THEME_DEFAULT_MARGIN_SMALL 4
#define THEME_DEFAULT_MARGIN_MEDIUM 8
#define THEME_DEFAULT_MARGIN_LARGE 16
#define THEME_DEFAULT_ANIMATION_DURATION 200
#define THEME_DEFAULT_TRANSPARENCY_LEVEL 0.9

// Color constants
#define THEME_COLOR_WHITE "#FFFFFF"
#define THEME_COLOR_BLACK "#000000"
#define THEME_COLOR_GRAY_LIGHT "#F5F5F5"
#define THEME_COLOR_GRAY_MEDIUM "#CCCCCC"
#define THEME_COLOR_GRAY_DARK "#666666"
#define THEME_COLOR_BLUE_PRIMARY "#2196F3"
#define THEME_COLOR_BLUE_SECONDARY "#1976D2"
#define THEME_COLOR_GREEN_SUCCESS "#4CAF50"
#define THEME_COLOR_RED_ERROR "#F44336"
#define THEME_COLOR_ORANGE_WARNING "#FF9800"
#define THEME_COLOR_PURPLE_ACCENT "#9C27B0"

// File paths
#define THEME_CONFIG_FILE "data/theme.conf"
#define THEME_CSS_DIR "assets/css/"
#define THEME_LIGHT_CSS_FILE "assets/css/light.css"
#define THEME_DARK_CSS_FILE "assets/css/dark.css"
#define THEME_PRESETS_DIR "data/themes/"

// Theme names
#define THEME_NAME_LIGHT "Light"
#define THEME_NAME_DARK "Dark"
#define THEME_NAME_AUTO "Auto"
#define THEME_NAME_HIGH_CONTRAST "High Contrast"
#define THEME_NAME_MINIMAL "Minimal"

#endif // THEME_H
