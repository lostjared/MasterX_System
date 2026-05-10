#include "window.hpp"
#include "matrix.hpp"

namespace mx {

void mxApp::resize(int w, int h) {
  SDL_SetWindowSize(win, w, h);
  width = w;
  height = h;
  if (ren)
    ren->setLogicalSize(w, h);
  if (tex) {
    DestroyTexture(tex);
    tex = nullptr;
  }
  if (ren) {
    tex = ren->createTarget(w, h);
    if (tex == nullptr) {
      mx::system_err << "GLContext::createTarget failed\n";
      SDL_DestroyWindow(win);
      SDL_Quit();
      exit(EXIT_FAILURE);
    }
  }
}

bool mxApp::init(const std::string &name, int w, int h) {

  config.loadFile(getPath("mx.cfg"));
  version = config.itemAtKey("app", "version").value;
  system_font = config.itemAtKey("fonts", "system").value;
  term_font = config.itemAtKey("fonts", "term").value;
  matrix_font = config.itemAtKey("fonts", "matrix").value;

  matrix_font_ = loadFont(matrix_font, 14);

  mx::system_out << "MasterX System v" << version << " Loading .... \n";
  mx::system_out << "(C) 2026 LostSideDead Software\n";

  // Pure-GL backend: create an SDL window with SDL_WINDOW_OPENGL,
  // then build our own GL context via mx::GLContext.
  win = SDL_CreateWindow(
      name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h,
      SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
  if (win == nullptr) {
    mx::system_err << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
    SDL_Quit();
    return false;
  }

  ren = new mx::GLContext();
  if (!ren->init(win)) {
    mx::system_err << "GLContext init failed\n";
    SDL_DestroyWindow(win);
    SDL_Quit();
    return false;
  }

  tex = ren->createTarget(w, h);
  if (tex == nullptr) {
    mx::system_err << "GLContext::createTarget failed\n";
    delete ren;
    ren = nullptr;
    SDL_DestroyWindow(win);
    SDL_Quit();
    return false;
  }
  init_ = true;
  width = w;
  height = h;

  font = TTF_OpenFont(getPath(system_font).c_str(), 14);
  if (!font) {
    mx::system_err << "MasterX System: font: " << getPath(system_font)
                   << " Could not be loaded.\n";
    mx::system_err.flush();
    exit(EXIT_FAILURE);
  }
  icon = loadTexture(*this, "images/xicon.bmp");
  SDL_Surface *ico = SDL_LoadBMP(getPath("images/xicon.bmp").c_str());
  SDL_SetWindowIcon(win, ico);
  SDL_FreeSurface(ico);
  mx::system_out << "MasterX: Initalized System Objects\n";
  return true;
}

SDL_Texture *mxApp::convertToStreamingTexture(SDL_Texture *originalTexture) {
  // Streaming textures aren't actually used in the GL backend.
  // Just return the original texture so callers that retain the
  // pointer keep working.
  return originalTexture;
}

void mxApp::release() {
  if (init_ == true) {
    releaseMatrix();
    mx::system_out << "MasterX: Releasing System Objects\n";
    if (icon) {
      DestroyTexture(icon);
      icon = nullptr;
    }
    if (tex) {
      DestroyTexture(tex);
      tex = nullptr;
    }
    if (ren) {
      delete ren;
      ren = nullptr;
    }
    SDL_DestroyWindow(win);
    init_ = false;
  }
}

void mxApp::shutdown() { active = false; }

void mxApp::set_fullscreen(SDL_Window *window, bool fullscreen) {
  if (fullscreen) {
    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
  } else {
    SDL_SetWindowFullscreen(window, 0);
  }
  full = fullscreen;
}

void mxApp::font_printText(TTF_Font *font, int x, int y,
                           const std::string &text, const SDL_Color col) {
  SDL_Surface *surf = TTF_RenderText_Blended(font, text.c_str(), col);
  if (!surf) {
    mx::system_err << "MasterX: System Font Render failed...\n";
    mx::system_err.flush();
    exit(EXIT_FAILURE);
  }
  SDL_Texture *tex_ = SDL_CreateTextureFromSurface(ren, surf);
  if (!tex_) {
    mx::system_err << "MastesrX System: Could not create texture..\n";
    mx::system_err.flush();
    SDL_FreeSurface(surf);
    exit(EXIT_FAILURE);
  }
  SDL_Rect rc = {x, y, surf->w, surf->h};
  SDL_RenderCopy(ren, tex_, nullptr, &rc);
  SDL_FreeSurface(surf);
  SDL_DestroyTexture(tex_);
}
void mxApp::font_printText_Solid(TTF_Font *font, int x, int y,
                                 const std::string &text, const SDL_Color col) {
  SDL_Surface *surf = TTF_RenderText_Solid(font, text.c_str(), col);
  if (!surf) {
    mx::system_err << "MasterX: System Font Render failed...\n";
    mx::system_err.flush();
    exit(EXIT_FAILURE);
  }
  SDL_Texture *tex_ = SDL_CreateTextureFromSurface(ren, surf);
  if (!tex_) {
    mx::system_err << "MastesrX System: Could not create texture..\n";
    mx::system_err.flush();
    SDL_FreeSurface(surf);
    exit(EXIT_FAILURE);
  }
  SDL_Rect rc = {x, y, surf->w, surf->h};
  SDL_RenderCopy(ren, tex_, nullptr, &rc);
  SDL_FreeSurface(surf);
  SDL_DestroyTexture(tex_);
}

void mxApp::printText(int x, int y, const std::string &text,
                      const SDL_Color col) {
  font_printText(font, x, y, text, col);
}

void mxApp::printText_Solid(int x, int y, const std::string &text,
                            const SDL_Color col) {
  font_printText_Solid(font, x, y, text, col);
}

TTF_Font *mxApp::loadFont(const std::string &font, const int size) {
  TTF_Font *fnt = TTF_OpenFont(getPath(font).c_str(), size);
  if (!fnt) {
    mx::system_err << "MasterX System: Error loading font..: " << getPath(font)
                   << "\n";
    mx::system_err.flush();
    exit(EXIT_FAILURE);
  }
  return fnt;
}

mxApp::~mxApp() {
  release();
  TTF_CloseFont(font);
  TTF_CloseFont(matrix_font_);
}
} // namespace mx