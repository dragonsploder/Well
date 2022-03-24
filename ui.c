#include "well.h"

#define NK_IMPLEMENTATION
#include "nuklear.h"
#define NK_GLFW_GL3_IMPLEMENTATION
#include "nuklear_glfw_gl3.h"

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

void initUI(struct UI* ui, GLFWwindow* window) {
    GL_CALL(glfwSetWindowUserPointer(window, &ui->glfw));
    ui->glfw.win = window;
    nk_init_default(&ui->glfw.ctx, 0);
    ui->glfw.ctx.clip.userdata = nk_handle_ptr(&ui->glfw);
    ui->glfw.last_button_click = 0;
    
    nk_glfw3_device_create(&ui->glfw);

    ui->glfw.is_double_click_down = nk_false;
    ui->glfw.double_click_pos = nk_vec2(0, 0);

    {struct nk_font_atlas *atlas;
    nk_font_atlas_init_default(&ui->glfw.atlas);
    nk_font_atlas_begin(&ui->glfw.atlas);
    atlas = &ui->glfw.atlas;
    struct nk_font *droid = nk_font_atlas_add_from_file(atlas, "../../resources/fonts/DroidSans.ttf", 28, 0);
    const void *image; 
    int w;
    int h;
    image = nk_font_atlas_bake(&ui->glfw.atlas, &w, &h, NK_FONT_ATLAS_RGBA32);
    struct nk_glfw_device *dev = &ui->glfw.ogl;
    glGenTextures(1, &dev->font_tex);
    glBindTexture(GL_TEXTURE_2D, dev->font_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)w, (GLsizei)h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    nk_font_atlas_end(&ui->glfw.atlas, nk_handle_id((int)ui->glfw.ogl.font_tex), &ui->glfw.ogl.null);
    if (ui->glfw.atlas.default_font)
        nk_style_set_font(&ui->glfw.ctx, &ui->glfw.atlas.default_font->handle);
    nk_style_set_font(&ui->glfw.ctx, &droid->handle);}

    ui->glfw.text_len = 0;
}


void newUIFrame(struct UI* ui, struct GameState* gameState) {
    nk_glfw3_new_frame(&ui->glfw);
    struct nk_colorf pallet1;
    pallet1.r = gameState->currentPallet[0][0], pallet1.g = gameState->currentPallet[0][1], pallet1.b = gameState->currentPallet[0][2], pallet1.a = 1.0f;
    struct nk_colorf pallet2;
    pallet2.r = gameState->currentPallet[1][0], pallet2.g = gameState->currentPallet[1][1], pallet2.b = gameState->currentPallet[1][2], pallet2.a = 1.0f;
    struct nk_colorf pallet3;
    pallet3.r = gameState->currentPallet[2][0], pallet3.g = gameState->currentPallet[2][1], pallet3.b = gameState->currentPallet[2][2], pallet3.a = 1.0f;
    struct nk_colorf pallet4;
    pallet4.r = gameState->currentPallet[3][0], pallet4.g = gameState->currentPallet[3][1], pallet4.b = gameState->currentPallet[3][2], pallet4.a = 1.0f;
    
    if (nk_begin(&ui->glfw.ctx, "Debug", nk_rect(50, 50, 330, 350),
        NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
        NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE))
    {
        nk_layout_row_dynamic(&ui->glfw.ctx, 25, 1);
        nk_value_float(&ui->glfw.ctx, "FPS:", gameState->currentFPS);

        nk_layout_row_dynamic(&ui->glfw.ctx, 25, 1);
        nk_property_int(&ui->glfw.ctx, "Pixelate", 1, &gameState->actualToPixelConvertion, 100, 1, 0.5);

        
        nk_layout_row_dynamic(&ui->glfw.ctx, 20, 1);
        nk_label(&ui->glfw.ctx, "Pallet Dark:", NK_TEXT_LEFT);
        nk_layout_row_dynamic(&ui->glfw.ctx, 25, 1);
        if (nk_combo_begin_color(&ui->glfw.ctx, nk_rgb_cf(pallet1), nk_vec2(nk_widget_width(&ui->glfw.ctx), 400))) {
            nk_layout_row_dynamic(&ui->glfw.ctx, 120, 1);
            pallet1 = nk_color_picker(&ui->glfw.ctx, pallet1, NK_RGBA);
            nk_layout_row_dynamic(&ui->glfw.ctx, 25, 1);
            pallet1.r = nk_propertyf(&ui->glfw.ctx, "#R:", 0, pallet1.r, 1.0f, 0.01f,0.005f);
            pallet1.g = nk_propertyf(&ui->glfw.ctx, "#G:", 0, pallet1.g, 1.0f, 0.01f,0.005f);
            pallet1.b = nk_propertyf(&ui->glfw.ctx, "#B:", 0, pallet1.b, 1.0f, 0.01f,0.005f);
            pallet1.a = nk_propertyf(&ui->glfw.ctx, "#A:", 0, pallet1.a, 1.0f, 0.01f,0.005f);
            nk_combo_end(&ui->glfw.ctx);
        }

        nk_layout_row_dynamic(&ui->glfw.ctx, 20, 1);
        nk_label(&ui->glfw.ctx, "Pallet Dark Mid:", NK_TEXT_LEFT);
        nk_layout_row_dynamic(&ui->glfw.ctx, 25, 1);
        if (nk_combo_begin_color(&ui->glfw.ctx, nk_rgb_cf(pallet2), nk_vec2(nk_widget_width(&ui->glfw.ctx), 400))) {
            nk_layout_row_dynamic(&ui->glfw.ctx, 120, 1);
            pallet2 = nk_color_picker(&ui->glfw.ctx, pallet2, NK_RGBA);
            nk_layout_row_dynamic(&ui->glfw.ctx, 25, 1);
            pallet2.r = nk_propertyf(&ui->glfw.ctx, "#R:", 0, pallet2.r, 1.0f, 0.01f,0.005f);
            pallet2.g = nk_propertyf(&ui->glfw.ctx, "#G:", 0, pallet2.g, 1.0f, 0.01f,0.005f);
            pallet2.b = nk_propertyf(&ui->glfw.ctx, "#B:", 0, pallet2.b, 1.0f, 0.01f,0.005f);
            pallet2.a = nk_propertyf(&ui->glfw.ctx, "#A:", 0, pallet2.a, 1.0f, 0.01f,0.005f);
            nk_combo_end(&ui->glfw.ctx);
        }

        nk_layout_row_dynamic(&ui->glfw.ctx, 20, 1);
        nk_label(&ui->glfw.ctx, "Pallet Light Mid:", NK_TEXT_LEFT);
        nk_layout_row_dynamic(&ui->glfw.ctx, 25, 1);
        if (nk_combo_begin_color(&ui->glfw.ctx, nk_rgb_cf(pallet3), nk_vec2(nk_widget_width(&ui->glfw.ctx), 400))) {
            nk_layout_row_dynamic(&ui->glfw.ctx, 120, 1);
            pallet3 = nk_color_picker(&ui->glfw.ctx, pallet3, NK_RGBA);
            nk_layout_row_dynamic(&ui->glfw.ctx, 25, 1);
            pallet3.r = nk_propertyf(&ui->glfw.ctx, "#R:", 0, pallet3.r, 1.0f, 0.01f,0.005f);
            pallet3.g = nk_propertyf(&ui->glfw.ctx, "#G:", 0, pallet3.g, 1.0f, 0.01f,0.005f);
            pallet3.b = nk_propertyf(&ui->glfw.ctx, "#B:", 0, pallet3.b, 1.0f, 0.01f,0.005f);
            pallet3.a = nk_propertyf(&ui->glfw.ctx, "#A:", 0, pallet3.a, 1.0f, 0.01f,0.005f);
            nk_combo_end(&ui->glfw.ctx);
        }

        nk_layout_row_dynamic(&ui->glfw.ctx, 20, 1);
        nk_label(&ui->glfw.ctx, "Pallet Light:", NK_TEXT_LEFT);
        nk_layout_row_dynamic(&ui->glfw.ctx, 25, 1);
        if (nk_combo_begin_color(&ui->glfw.ctx, nk_rgb_cf(pallet4), nk_vec2(nk_widget_width(&ui->glfw.ctx), 400))) {
            nk_layout_row_dynamic(&ui->glfw.ctx, 120, 1);
            pallet4 = nk_color_picker(&ui->glfw.ctx, pallet4, NK_RGBA);
            nk_layout_row_dynamic(&ui->glfw.ctx, 25, 1);
            pallet4.r = nk_propertyf(&ui->glfw.ctx, "#R:", 0, pallet4.r, 1.0f, 0.01f,0.005f);
            pallet4.g = nk_propertyf(&ui->glfw.ctx, "#G:", 0, pallet4.g, 1.0f, 0.01f,0.005f);
            pallet4.b = nk_propertyf(&ui->glfw.ctx, "#B:", 0, pallet4.b, 1.0f, 0.01f,0.005f);
            pallet4.a = nk_propertyf(&ui->glfw.ctx, "#A:", 0, pallet4.a, 1.0f, 0.01f,0.005f);
            nk_combo_end(&ui->glfw.ctx);
        }
        
    }
    nk_end(&ui->glfw.ctx);

    gameState->currentPallet[0][0] = pallet1.r, gameState->currentPallet[0][1] = pallet1.g, gameState->currentPallet[0][2] = pallet1.b;
    gameState->currentPallet[1][0] = pallet2.r, gameState->currentPallet[1][1] = pallet2.g, gameState->currentPallet[1][2] = pallet2.b;
    gameState->currentPallet[2][0] = pallet3.r, gameState->currentPallet[2][1] = pallet3.g, gameState->currentPallet[2][2] = pallet3.b;
    gameState->currentPallet[3][0] = pallet4.r, gameState->currentPallet[3][1] = pallet4.g, gameState->currentPallet[3][2] = pallet4.b;
}

void renderUI(struct UI* ui) {
    nk_glfw3_render(&ui->glfw, NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
}

void freeUI(struct UI* ui) {
    nk_glfw3_shutdown(&ui->glfw);
}