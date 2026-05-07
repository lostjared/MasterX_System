#ifndef MX_GL_CONTEXT_HPP
#define MX_GL_CONTEXT_HPP

// Pure-GL replacement for SDL_Renderer / SDL_Texture used throughout
// MasterX.  All drawing goes through mx::GLContext, which owns an
// SDL_GLContext and a small set of GLES2-flavoured shaders.  Textures are
// owned by mx::Texture (a wrapper holding a GL texture id, optional FBO
// for render-target use, and the original SDL_Surface for reference).
//
// Compatibility macros in mx_render_compat.hpp redirect existing
// SDL_Render*/SDL_Texture/SDL_Renderer call sites to this implementation,
// so the rest of the codebase keeps the SDL_Renderer-shaped API.

#include "SDL.h"

namespace mx {

    struct Texture {
        unsigned int id = 0;     // GL texture id
        unsigned int fbo = 0;    // non-zero when this texture is a render target
        int w = 0;
        int h = 0;
        bool ownsId = true;
        // Modulation state (mirrors SDL_SetTextureAlphaMod / ColorMod)
        Uint8 rMod = 255;
        Uint8 gMod = 255;
        Uint8 bMod = 255;
        Uint8 aMod = 255;
        SDL_BlendMode blend = SDL_BLENDMODE_BLEND;
    };

    class GLContext {
    public:
        GLContext() = default;
        ~GLContext();

        bool init(SDL_Window *win);
        void shutdown();
        bool isReady() const { return ready_; }

        // Lifecycle / framebuffer
        void present();
        void setLogicalSize(int w, int h);
        void getOutputSize(int *w, int *h) const;
        SDL_Window *window() const { return win_; }

        // State
        void setDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
        SDL_Color drawColor() const { return drawCol_; }
        void setBlendMode(SDL_BlendMode m);
        void setClipRect(const SDL_Rect *rect);
        void setRenderTarget(Texture *t);
        Texture *renderTarget() const { return target_; }

        // Drawing primitives
        void clear();
        void fillRect(const SDL_Rect *rect);
        void drawRect(const SDL_Rect *rect);
        void drawLine(int x1, int y1, int x2, int y2);
        void drawPoint(int x, int y);
        void copy(Texture *t, const SDL_Rect *src, const SDL_Rect *dst);

        // Texture creation
        Texture *createFromSurface(SDL_Surface *surf);
        Texture *createTarget(int w, int h);
        Texture *createStreaming(int w, int h);
        void destroyTexture(Texture *t);

        // Effect shader support.
        // Build a custom shader reusing the built-in vertex shader + a custom
        // GLES2 fragment shader source.  Returns 0 on failure; the caller
        // owns the returned GL program id (call glDeleteProgram when done).
        unsigned int buildEffectShader(const char *fragmentSrc);

        // Render texture t full-screen through prog.
        // Uniforms supplied automatically: uProj (vec4), uTex (sampler2D, slot 0),
        // uTime (float), uResolution (vec2).
        void applyEffect(Texture *t, unsigned int prog, float time);

        // Internal: bind whatever framebuffer corresponds to the current
        // render target and apply the matching viewport.
        void applyTarget();

    private:
        SDL_Window *win_ = nullptr;
        SDL_GLContext glctx_ = nullptr;

        // Output / logical sizing
        int outW_ = 0, outH_ = 0;
        int logicalW_ = 0, logicalH_ = 0;

        // Solid-color shader
        unsigned int progSolid_ = 0;
        int locSolidProj_ = -1;
        int locSolidColor_ = -1;

        // Textured shader (with color + alpha modulation)
        unsigned int progTex_ = 0;
        int locTexProj_ = -1;
        int locTexSampler_ = -1;
        int locTexMod_ = -1;

        unsigned int vbo_ = 0;
        bool ready_ = false;

        // Render state
        SDL_Color drawCol_ = {255, 255, 255, 255};
        SDL_BlendMode blend_ = SDL_BLENDMODE_BLEND;
        bool clipEnabled_ = false;
        SDL_Rect clip_{0, 0, 0, 0};
        Texture *target_ = nullptr;

        bool buildShaders();
        void useSolid();
        void useTextured(Texture *t);
        void uploadProj(int locProj);
        void issueQuad(float x0, float y0, float x1, float y1,
                       float u0, float v0, float u1, float v1);
        int  currentTargetW() const;
        int  currentTargetH() const;
        bool currentTargetFlipY() const; // true when drawing to default FB
    };

} // namespace mx

#endif
