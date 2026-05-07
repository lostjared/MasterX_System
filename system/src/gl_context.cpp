#include "gl_context.hpp"
#include "mx_render_compat.hpp"
#include "tee_stream.hpp"
#include <cstring>
#include <string>
#include <vector>

#ifdef FOR_WASM
#include <SDL_opengles2.h>
#else
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#endif

namespace mx {

    // ---------------------------------------------------------------------
    // Shader sources (GLES2 / GLSL 1.00 — works on desktop GL too via the
    // GL_ES guard).
    // ---------------------------------------------------------------------
    namespace {

        const char *kVS =
            "#ifdef GL_ES\n"
            "precision mediump float;\n"
            "#endif\n"
            "attribute vec2 aPos;\n"
            "attribute vec2 aUV;\n"
            "uniform vec4 uProj;\n"  // (2/w, 2/h, -1 or +1 for flipY, -1 or +1)
            "varying vec2 vUV;\n"
            "void main() {\n"
            "    float ndcX = aPos.x * uProj.x - 1.0;\n"
            "    float ndcY = aPos.y * uProj.y * uProj.w + uProj.z;\n"
            "    gl_Position = vec4(ndcX, ndcY, 0.0, 1.0);\n"
            "    vUV = aUV;\n"
            "}\n";

        const char *kFSSolid =
            "#ifdef GL_ES\n"
            "precision mediump float;\n"
            "#endif\n"
            "uniform vec4 uColor;\n"
            "varying vec2 vUV;\n"
            "void main() {\n"
            "    gl_FragColor = uColor;\n"
            "}\n";

        const char *kFSTex =
            "#ifdef GL_ES\n"
            "precision mediump float;\n"
            "#endif\n"
            "uniform sampler2D uTex;\n"
            "uniform vec4 uMod;\n"  // rgba modulation 0..1
            "varying vec2 vUV;\n"
            "void main() {\n"
            "    vec4 c = texture2D(uTex, vUV);\n"
            "    gl_FragColor = c * uMod;\n"
            "}\n";

        unsigned int compile(unsigned int type, const char *src) {
            unsigned int sh = glCreateShader(type);
            glShaderSource(sh, 1, &src, nullptr);
            glCompileShader(sh);
            GLint ok = 0;
            glGetShaderiv(sh, GL_COMPILE_STATUS, &ok);
            if (!ok) {
                char log[1024] = {0};
                GLsizei n = 0;
                glGetShaderInfoLog(sh, sizeof(log) - 1, &n, log);
                mx::system_err << "GLContext: shader compile error: "
                               << log << "\n";
                glDeleteShader(sh);
                return 0;
            }
            return sh;
        }

        unsigned int link(unsigned int vs, unsigned int fs) {
            unsigned int p = glCreateProgram();
            glAttachShader(p, vs);
            glAttachShader(p, fs);
            glBindAttribLocation(p, 0, "aPos");
            glBindAttribLocation(p, 1, "aUV");
            glLinkProgram(p);
            GLint ok = 0;
            glGetProgramiv(p, GL_LINK_STATUS, &ok);
            if (!ok) {
                char log[1024] = {0};
                GLsizei n = 0;
                glGetProgramInfoLog(p, sizeof(log) - 1, &n, log);
                mx::system_err << "GLContext: program link error: "
                               << log << "\n";
                glDeleteProgram(p);
                return 0;
            }
            return p;
        }

    } // namespace

    // ---------------------------------------------------------------------
    // Setup / teardown
    // ---------------------------------------------------------------------
    GLContext::~GLContext() { shutdown(); }

    bool GLContext::init(SDL_Window *win) {
        win_ = win;

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                            SDL_GL_CONTEXT_PROFILE_ES);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

        glctx_ = SDL_GL_CreateContext(win_);
        if (!glctx_) {
            mx::system_err << "SDL_GL_CreateContext failed: "
                           << SDL_GetError() << "\n";
            return false;
        }
        if (SDL_GL_MakeCurrent(win_, glctx_) != 0) {
            mx::system_err << "SDL_GL_MakeCurrent failed: "
                           << SDL_GetError() << "\n";
            return false;
        }
        SDL_GL_SetSwapInterval(1);

        if (!buildShaders()) return false;

        glGenBuffers(1, &vbo_);

        SDL_GL_GetDrawableSize(win_, &outW_, &outH_);
        logicalW_ = outW_;
        logicalH_ = outH_;

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glViewport(0, 0, outW_, outH_);

        ready_ = true;
        mx::system_out << "MasterX GL: context ready (" << outW_ << "x"
                       << outH_ << ")\n";
        return true;
    }

    bool GLContext::buildShaders() {
        unsigned int vs = compile(GL_VERTEX_SHADER, kVS);
        if (!vs) return false;
        unsigned int fsSolid = compile(GL_FRAGMENT_SHADER, kFSSolid);
        unsigned int fsTex = compile(GL_FRAGMENT_SHADER, kFSTex);
        if (!fsSolid || !fsTex) {
            glDeleteShader(vs);
            if (fsSolid) glDeleteShader(fsSolid);
            if (fsTex) glDeleteShader(fsTex);
            return false;
        }
        progSolid_ = link(vs, fsSolid);
        progTex_ = link(vs, fsTex);
        glDeleteShader(vs);
        glDeleteShader(fsSolid);
        glDeleteShader(fsTex);
        if (!progSolid_ || !progTex_) return false;

        locSolidProj_  = glGetUniformLocation(progSolid_, "uProj");
        locSolidColor_ = glGetUniformLocation(progSolid_, "uColor");
        locTexProj_    = glGetUniformLocation(progTex_, "uProj");
        locTexSampler_ = glGetUniformLocation(progTex_, "uTex");
        locTexMod_     = glGetUniformLocation(progTex_, "uMod");
        return true;
    }

    void GLContext::shutdown() {
        if (vbo_) { glDeleteBuffers(1, &vbo_); vbo_ = 0; }
        if (progSolid_) { glDeleteProgram(progSolid_); progSolid_ = 0; }
        if (progTex_)   { glDeleteProgram(progTex_); progTex_ = 0; }
        if (glctx_) { SDL_GL_DeleteContext(glctx_); glctx_ = nullptr; }
        ready_ = false;
        win_ = nullptr;
    }

    // ---------------------------------------------------------------------
    // State
    // ---------------------------------------------------------------------
    void GLContext::setDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
        drawCol_ = {r, g, b, a};
    }

    void GLContext::setBlendMode(SDL_BlendMode m) {
        blend_ = m;
        if (m == SDL_BLENDMODE_NONE) {
            glDisable(GL_BLEND);
        } else {
            glEnable(GL_BLEND);
            switch (m) {
                case SDL_BLENDMODE_ADD:
                    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
                    break;
                case SDL_BLENDMODE_MOD:
                    glBlendFunc(GL_DST_COLOR, GL_ZERO);
                    break;
                case SDL_BLENDMODE_BLEND:
                default:
                    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                    break;
            }
        }
    }

    void GLContext::setClipRect(const SDL_Rect *rect) {
        if (!rect) {
            clipEnabled_ = false;
            glDisable(GL_SCISSOR_TEST);
            return;
        }
        clipEnabled_ = true;
        clip_ = *rect;
        glEnable(GL_SCISSOR_TEST);
        // GL scissor uses bottom-left origin when drawing to default FB.
        const int th = currentTargetH();
        if (currentTargetFlipY()) {
            glScissor(rect->x, th - (rect->y + rect->h),
                      rect->w, rect->h);
        } else {
            glScissor(rect->x, rect->y, rect->w, rect->h);
        }
    }

    void GLContext::setRenderTarget(Texture *t) {
        target_ = t;
        applyTarget();
    }

    int GLContext::currentTargetW() const {
        return target_ ? target_->w : outW_;
    }

    int GLContext::currentTargetH() const {
        return target_ ? target_->h : outH_;
    }

    bool GLContext::currentTargetFlipY() const {
        return target_ == nullptr;
    }

    void GLContext::applyTarget() {
        if (target_ && target_->fbo) {
            glBindFramebuffer(GL_FRAMEBUFFER, target_->fbo);
            glViewport(0, 0, target_->w, target_->h);
        } else {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, outW_, outH_);
        }
        if (clipEnabled_) {
            // Re-apply scissor in target's coordinate space.
            setClipRect(&clip_);
        }
    }

    void GLContext::setLogicalSize(int w, int h) {
        SDL_GL_GetDrawableSize(win_, &outW_, &outH_);
        logicalW_ = w > 0 ? w : outW_;
        logicalH_ = h > 0 ? h : outH_;
        if (!target_) glViewport(0, 0, outW_, outH_);
    }

    void GLContext::getOutputSize(int *w, int *h) const {
        if (w) *w = outW_;
        if (h) *h = outH_;
    }

    void GLContext::present() {
        SDL_GL_SwapWindow(win_);
    }

    // ---------------------------------------------------------------------
    // Primitive drawing
    // ---------------------------------------------------------------------
    void GLContext::uploadProj(int locProj) {
        const int tw = currentTargetW();
        const int th = currentTargetH();
        if (tw <= 0 || th <= 0) return;
        // uProj.xy = pixel->NDC scale, .z = NDC y offset, .w = y flip
        // Use NDC y = aPos.y * (2/h) * flip + offset.
        const float flip = currentTargetFlipY() ? -1.0f : 1.0f;
        const float yOff = currentTargetFlipY() ? 1.0f : -1.0f;
        glUniform4f(locProj,
                    2.0f / static_cast<float>(tw),
                    2.0f / static_cast<float>(th),
                    yOff,
                    flip);
    }

    void GLContext::useSolid() {
        glUseProgram(progSolid_);
        uploadProj(locSolidProj_);
    }

    void GLContext::useTextured(Texture *t) {
        glUseProgram(progTex_);
        uploadProj(locTexProj_);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, t->id);
        if (locTexSampler_ >= 0) glUniform1i(locTexSampler_, 0);
        if (locTexMod_ >= 0) {
            glUniform4f(locTexMod_,
                        t->rMod / 255.0f,
                        t->gMod / 255.0f,
                        t->bMod / 255.0f,
                        t->aMod / 255.0f);
        }
    }

    void GLContext::issueQuad(float x0, float y0, float x1, float y1,
                              float u0, float v0, float u1, float v1) {
        const float verts[] = {
            x0, y0, u0, v0,
            x1, y0, u1, v0,
            x0, y1, u0, v1,
            x1, y1, u1, v1,
        };
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STREAM_DRAW);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
                              4 * sizeof(float),
                              reinterpret_cast<const void *>(0));
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
                              4 * sizeof(float),
                              reinterpret_cast<const void *>(2 * sizeof(float)));
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void GLContext::clear() {
        // Make sure the right framebuffer is bound (in case the user just
        // called setRenderTarget without issuing any other state).
        applyTarget();
        glClearColor(drawCol_.r / 255.0f, drawCol_.g / 255.0f,
                     drawCol_.b / 255.0f, drawCol_.a / 255.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void GLContext::fillRect(const SDL_Rect *rect) {
        applyTarget();
        useSolid();
        if (locSolidColor_ >= 0) {
            glUniform4f(locSolidColor_,
                        drawCol_.r / 255.0f,
                        drawCol_.g / 255.0f,
                        drawCol_.b / 255.0f,
                        drawCol_.a / 255.0f);
        }
        SDL_Rect r = rect ? *rect : SDL_Rect{0, 0, currentTargetW(), currentTargetH()};
        issueQuad(static_cast<float>(r.x), static_cast<float>(r.y),
                  static_cast<float>(r.x + r.w), static_cast<float>(r.y + r.h),
                  0, 0, 1, 1);
    }

    void GLContext::drawRect(const SDL_Rect *rect) {
        if (!rect) return;
        // Outline: 4 thin filled rects 1px each.
        SDL_Rect top{rect->x, rect->y, rect->w, 1};
        SDL_Rect bottom{rect->x, rect->y + rect->h - 1, rect->w, 1};
        SDL_Rect left{rect->x, rect->y, 1, rect->h};
        SDL_Rect right{rect->x + rect->w - 1, rect->y, 1, rect->h};
        fillRect(&top);
        fillRect(&bottom);
        fillRect(&left);
        fillRect(&right);
    }

    void GLContext::drawLine(int x1, int y1, int x2, int y2) {
        // Thick lines aren't reliably supported in GLES; for the only two
        // axis-aligned use cases (caret + arrow chrome) approximate with a
        // 1-pixel rect along the dominant axis. For diagonal lines fall
        // back to GL_LINES with width 1.
        if (x1 == x2) {
            int y0 = y1 < y2 ? y1 : y2;
            int yh = (y1 < y2 ? y2 - y1 : y1 - y2) + 1;
            SDL_Rect r{x1, y0, 1, yh};
            fillRect(&r);
            return;
        }
        if (y1 == y2) {
            int x0 = x1 < x2 ? x1 : x2;
            int xw = (x1 < x2 ? x2 - x1 : x1 - x2) + 1;
            SDL_Rect r{x0, y1, xw, 1};
            fillRect(&r);
            return;
        }
        applyTarget();
        useSolid();
        if (locSolidColor_ >= 0) {
            glUniform4f(locSolidColor_,
                        drawCol_.r / 255.0f,
                        drawCol_.g / 255.0f,
                        drawCol_.b / 255.0f,
                        drawCol_.a / 255.0f);
        }
        const float verts[] = {
            static_cast<float>(x1), static_cast<float>(y1), 0.0f, 0.0f,
            static_cast<float>(x2), static_cast<float>(y2), 1.0f, 1.0f,
        };
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STREAM_DRAW);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
                              4 * sizeof(float),
                              reinterpret_cast<const void *>(0));
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
                              4 * sizeof(float),
                              reinterpret_cast<const void *>(2 * sizeof(float)));
        glDrawArrays(GL_LINES, 0, 2);
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void GLContext::drawPoint(int x, int y) {
        SDL_Rect r{x, y, 1, 1};
        fillRect(&r);
    }

    void GLContext::copy(Texture *t, const SDL_Rect *src, const SDL_Rect *dst) {
        if (!t || !t->id) return;
        applyTarget();
        // Honor the texture's own blend mode, like SDL_Renderer does, so
        // text/icons drawn after a BLENDMODE_NONE pass don't suddenly
        // render as opaque blocks.
        SDL_BlendMode prev = blend_;
        if (t->blend != blend_) setBlendMode(t->blend);
        useTextured(t);
        SDL_Rect d = dst ? *dst
                         : SDL_Rect{0, 0, currentTargetW(), currentTargetH()};
        float u0 = 0.0f, v0 = 0.0f, u1 = 1.0f, v1 = 1.0f;
        if (src && t->w > 0 && t->h > 0) {
            u0 = static_cast<float>(src->x) / t->w;
            v0 = static_cast<float>(src->y) / t->h;
            u1 = static_cast<float>(src->x + src->w) / t->w;
            v1 = static_cast<float>(src->y + src->h) / t->h;
        }
        issueQuad(static_cast<float>(d.x), static_cast<float>(d.y),
                  static_cast<float>(d.x + d.w),
                  static_cast<float>(d.y + d.h),
                  u0, v0, u1, v1);
        glBindTexture(GL_TEXTURE_2D, 0);
        if (prev != blend_) setBlendMode(prev);
    }

    // ---------------------------------------------------------------------
    // Texture creation
    // ---------------------------------------------------------------------
    Texture *GLContext::createFromSurface(SDL_Surface *surf) {
        if (!surf) return nullptr;
        SDL_Surface *converted = nullptr;
        SDL_Surface *use = surf;
        if (surf->format->format != SDL_PIXELFORMAT_ABGR8888) {
            converted = SDL_ConvertSurfaceFormat(surf,
                                                 SDL_PIXELFORMAT_ABGR8888, 0);
            if (!converted) return nullptr;
            use = converted;
        }
        Texture *t = new Texture();
        t->w = use->w;
        t->h = use->h;
        glGenTextures(1, &t->id);
        glBindTexture(GL_TEXTURE_2D, t->id);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, use->w, use->h, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, use->pixels);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_2D, 0);
        if (converted) SDL_FreeSurface(converted);
        return t;
    }

    Texture *GLContext::createTarget(int w, int h) {
        Texture *t = new Texture();
        t->w = w;
        t->h = h;
        glGenTextures(1, &t->id);
        glBindTexture(GL_TEXTURE_2D, t->id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glGenFramebuffers(1, &t->fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, t->fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, t->id, 0);
        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            mx::system_err << "GLContext::createTarget: incomplete FBO\n";
        }
        // Initialise with transparent black
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        // Restore previous framebuffer (default or current target)
        applyTarget();
        glBindTexture(GL_TEXTURE_2D, 0);
        return t;
    }

    Texture *GLContext::createStreaming(int w, int h) {
        // Streaming/Lock semantics aren't actually used in the codebase;
        // return a regular sampleable texture so the call is safe.
        Texture *t = new Texture();
        t->w = w;
        t->h = h;
        glGenTextures(1, &t->id);
        glBindTexture(GL_TEXTURE_2D, t->id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
        return t;
    }

    void GLContext::destroyTexture(Texture *t) {
        if (!t) return;
        if (t->fbo) glDeleteFramebuffers(1, &t->fbo);
        if (t->id && t->ownsId) glDeleteTextures(1, &t->id);
        delete t;
    }

    // =====================================================================
    // Compatibility shim wrappers
    // =====================================================================
    void DestroyRenderer(GLContext *r) { /* lifecycle owned by mxApp */ (void)r; }
    void DestroyTexture(Texture *t) {
        if (!t) return;
        if (t->fbo) glDeleteFramebuffers(1, &t->fbo);
        if (t->id && t->ownsId) glDeleteTextures(1, &t->id);
        delete t;
    }

    int RenderClear(GLContext *r)              { if (r) r->clear();           return 0; }
    void RenderPresent(GLContext *r)           { if (r) r->present(); }
    int RenderFlush(GLContext *r)              { (void)r; glFlush(); return 0; }

    int SetRenderDrawColor(GLContext *r, Uint8 R, Uint8 G, Uint8 B, Uint8 A) {
        if (r) r->setDrawColor(R, G, B, A);
        return 0;
    }
    int SetRenderDrawBlendMode(GLContext *r, SDL_BlendMode m) {
        if (r) r->setBlendMode(m);
        return 0;
    }
    int RenderSetClipRect(GLContext *r, const SDL_Rect *rect) {
        if (r) r->setClipRect(rect);
        return 0;
    }
    int RenderSetLogicalSize(GLContext *r, int w, int h) {
        if (r) r->setLogicalSize(w, h);
        return 0;
    }
    int GetRendererOutputSize(GLContext *r, int *w, int *h) {
        if (r) r->getOutputSize(w, h);
        return 0;
    }
    int SetRenderTarget(GLContext *r, Texture *t) {
        if (r) r->setRenderTarget(t);
        return 0;
    }
    Texture *GetRenderTarget(GLContext *r) {
        return r ? r->renderTarget() : nullptr;
    }

    int RenderFillRect(GLContext *r, const SDL_Rect *rect) {
        if (r) r->fillRect(rect);
        return 0;
    }
    int RenderDrawRect(GLContext *r, const SDL_Rect *rect) {
        if (r) r->drawRect(rect);
        return 0;
    }
    int RenderDrawLine(GLContext *r, int x1, int y1, int x2, int y2) {
        if (r) r->drawLine(x1, y1, x2, y2);
        return 0;
    }
    int RenderDrawPoint(GLContext *r, int x, int y) {
        if (r) r->drawPoint(x, y);
        return 0;
    }
    int RenderCopy(GLContext *r, Texture *t,
                   const SDL_Rect *src, const SDL_Rect *dst) {
        if (r) r->copy(t, src, dst);
        return 0;
    }

    Texture *CreateTextureFromSurface(GLContext *r, SDL_Surface *s) {
        return r ? r->createFromSurface(s) : nullptr;
    }
    Texture *CreateTexture(GLContext *r, Uint32 fmt, int access, int w, int h) {
        (void)fmt;
        if (!r) return nullptr;
        if (access == SDL_TEXTUREACCESS_TARGET) return r->createTarget(w, h);
        return r->createStreaming(w, h);
    }
    int QueryTexture(Texture *t, Uint32 *fmt, int *access, int *w, int *h) {
        if (fmt) *fmt = SDL_PIXELFORMAT_ABGR8888;
        if (access) *access = t && t->fbo ? SDL_TEXTUREACCESS_TARGET
                                          : SDL_TEXTUREACCESS_STATIC;
        if (w) *w = t ? t->w : 0;
        if (h) *h = t ? t->h : 0;
        return 0;
    }
    int SetTextureBlendMode(Texture *t, SDL_BlendMode m) {
        if (t) t->blend = m;
        return 0;
    }
    int SetTextureAlphaMod(Texture *t, Uint8 a) {
        if (t) t->aMod = a;
        return 0;
    }
    int SetTextureColorMod(Texture *t, Uint8 R, Uint8 G, Uint8 B) {
        if (t) { t->rMod = R; t->gMod = G; t->bMod = B; }
        return 0;
    }

} // namespace mx
