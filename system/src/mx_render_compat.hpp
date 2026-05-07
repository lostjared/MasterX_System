#ifndef MX_RENDER_COMPAT_HPP
#define MX_RENDER_COMPAT_HPP

// Compatibility shim that lets the existing SDL_Render*-shaped code keep
// compiling, while every call is actually serviced by mx::GLContext.
// Include AFTER <SDL.h>.  All call sites become free functions in the
// `mx` namespace plus a few preprocessor redirects.
//
// The macros are intentionally simple textual substitutions; they only
// affect code that includes window.hpp (which includes this header).

#include "gl_context.hpp"

namespace mx {

    // Renderer / texture lifecycle
    void DestroyRenderer(GLContext *r);
    void DestroyTexture(Texture *t);

    // Frame
    int  RenderClear(GLContext *r);
    void RenderPresent(GLContext *r);
    int  RenderFlush(GLContext *r);

    // State
    int  SetRenderDrawColor(GLContext *r, Uint8 R, Uint8 G, Uint8 B, Uint8 A);
    int  SetRenderDrawBlendMode(GLContext *r, SDL_BlendMode m);
    int  RenderSetClipRect(GLContext *r, const SDL_Rect *rect);
    int  RenderSetLogicalSize(GLContext *r, int w, int h);
    int  GetRendererOutputSize(GLContext *r, int *w, int *h);
    int  SetRenderTarget(GLContext *r, Texture *t);
    Texture *GetRenderTarget(GLContext *r);

    // Primitives
    int  RenderFillRect(GLContext *r, const SDL_Rect *rect);
    int  RenderDrawRect(GLContext *r, const SDL_Rect *rect);
    int  RenderDrawLine(GLContext *r, int x1, int y1, int x2, int y2);
    int  RenderDrawPoint(GLContext *r, int x, int y);
    int  RenderCopy(GLContext *r, Texture *t,
                    const SDL_Rect *src, const SDL_Rect *dst);

    // Texture
    Texture *CreateTextureFromSurface(GLContext *r, SDL_Surface *s);
    Texture *CreateTexture(GLContext *r, Uint32 fmt, int access, int w, int h);
    int  QueryTexture(Texture *t, Uint32 *fmt, int *access, int *w, int *h);
    int  SetTextureBlendMode(Texture *t, SDL_BlendMode m);
    int  SetTextureAlphaMod(Texture *t, Uint8 a);
    int  SetTextureColorMod(Texture *t, Uint8 R, Uint8 G, Uint8 B);

} // namespace mx

// Type redirects. Done AFTER <SDL.h> so SDL's own declarations are intact.
#define SDL_Renderer                  mx::GLContext
#define SDL_Texture                   mx::Texture

// Function redirects.
#define SDL_DestroyRenderer           mx::DestroyRenderer
#define SDL_DestroyTexture            mx::DestroyTexture

#define SDL_RenderClear               mx::RenderClear
#define SDL_RenderPresent             mx::RenderPresent
#define SDL_RenderFlush               mx::RenderFlush

#define SDL_SetRenderDrawColor        mx::SetRenderDrawColor
#define SDL_SetRenderDrawBlendMode    mx::SetRenderDrawBlendMode
#define SDL_RenderSetClipRect         mx::RenderSetClipRect
#define SDL_RenderSetLogicalSize      mx::RenderSetLogicalSize
#define SDL_GetRendererOutputSize     mx::GetRendererOutputSize
#define SDL_SetRenderTarget           mx::SetRenderTarget
#define SDL_GetRenderTarget           mx::GetRenderTarget

#define SDL_RenderFillRect            mx::RenderFillRect
#define SDL_RenderDrawRect            mx::RenderDrawRect
#define SDL_RenderDrawLine            mx::RenderDrawLine
#define SDL_RenderDrawPoint           mx::RenderDrawPoint
#define SDL_RenderCopy                mx::RenderCopy

#define SDL_CreateTextureFromSurface  mx::CreateTextureFromSurface
#define SDL_CreateTexture             mx::CreateTexture
#define SDL_QueryTexture              mx::QueryTexture
#define SDL_SetTextureBlendMode       mx::SetTextureBlendMode
#define SDL_SetTextureAlphaMod        mx::SetTextureAlphaMod
#define SDL_SetTextureColorMod        mx::SetTextureColorMod

#endif
