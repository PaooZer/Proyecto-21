#include <SDL2/SDL.h> // para manejar ventanas
#include <SDL_image.h> // cargar imagenes
#include <SDL_ttf.h> // incluir textos
#include <SDL2_gfxPrimitives.h> // dibujar botones redondos
#include <iostream>
#include <vector>
#include <string>

using namespace std;

enum EstadoJuego {
    MENU_INICIAL,
    MENU_PRINCIPAL,
    ACERCA_DE_NOSOTROS
};

// ***** PROTOTIPOS *****

bool inicializar();
void cerrar();
bool estaEncima(SDL_Rect rect, int mouseX, int mouseY);
void renderTexto(const char* texto, SDL_Rect& rect, SDL_Color color, SDL_Renderer* renderer, TTF_Font* font);
void dibujarBotonRedondeado(SDL_Renderer* renderer, SDL_Rect rect, SDL_Color color, const char* texto, SDL_Color colorTexto, TTF_Font* font);
void mostrarAcercaDe(SDL_Renderer* renderer, TTF_Font* font, SDL_Rect botonVolver, int mouseX, int mouseY); //hay que progamarla y agregar boton de volver

// ***** VARIABLES GLOBALES *****

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
TTF_Font* font = nullptr;
SDL_Texture* fondoTexture = nullptr;

// ***** MAIN *****
int main(int argc, char* argv[]) {
    if (!inicializar()) return 1;

    EstadoJuego estado = MENU_INICIAL;
    bool quit = false;

    SDL_Rect botonEmpezar = { 300, 360, 200, 60 };
    SDL_Rect botonNueva   = { 300, 360, 200, 60 };
    SDL_Rect botonCargar  = { 300, 440, 200, 60 };
    SDL_Rect botonAcerca  = { 260, 520, 280, 60 };
    SDL_Rect botonVolver  = { 50, 520, 150, 50 };

    while (!quit) {
        int mouseX, mouseY;
        SDL_Event event;
        SDL_GetMouseState(&mouseX, &mouseY);

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) quit = true;

            if (event.type == SDL_MOUSEBUTTONDOWN) {
                int x = event.button.x;
                int y = event.button.y;

                if (estado == MENU_INICIAL && estaEncima(botonEmpezar, x, y)) {
                    estado = MENU_PRINCIPAL;
                } else if (estado == MENU_PRINCIPAL) {
                    if (estaEncima(botonNueva, x, y)) cout << "Nueva partida\n";
                    if (estaEncima(botonCargar, x, y)) cout << "Cargar partida\n";
                    if (estaEncima(botonAcerca, x, y)) estado = ACERCA_DE_NOSOTROS;
                } else if (estado == ACERCA_DE_NOSOTROS && estaEncima(botonVolver, x, y)) {
                    estado = MENU_PRINCIPAL;
                }
            }
        }

        if (fondoTexture) SDL_RenderCopy(renderer, fondoTexture, NULL, NULL);
        else SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255), SDL_RenderClear(renderer);

        SDL_Color blanco = { 255, 255, 255 };
        SDL_Color negro = { 0, 0, 0 };

        if (estado == MENU_INICIAL) {
            SDL_Color hoverColor = estaEncima(botonEmpezar, mouseX, mouseY) 
                           ? SDL_Color{200, 200, 200, 255} 
                           : SDL_Color{100, 100, 100, 255};
            dibujarBotonRedondeado(renderer, botonEmpezar, hoverColor, "PLAY", blanco, font);
        } else if (estado == MENU_PRINCIPAL) {
            dibujarBotonRedondeado(renderer, botonNueva, estaEncima(botonNueva, mouseX, mouseY) ? SDL_Color{180, 180, 180, 255} : SDL_Color{100, 100, 100, 255}, "Nueva Partida", blanco, font);
            dibujarBotonRedondeado(renderer, botonCargar, estaEncima(botonCargar, mouseX, mouseY) ? SDL_Color{180, 180, 180, 255} : SDL_Color{100, 100, 100, 255}, "Cargar Partida", blanco, font);
            dibujarBotonRedondeado(renderer, botonAcerca, estaEncima(botonAcerca, mouseX, mouseY) ? SDL_Color{180, 180, 180, 255} : SDL_Color{100, 100, 100, 255}, "Acerca de Nosotras", blanco, font);
        } else if (estado == ACERCA_DE_NOSOTROS) {
            mostrarAcercaDe(renderer, font, botonVolver, mouseX, mouseY); //asi se deberia de llamar (solo programarla y verificar)
        }

        SDL_RenderPresent(renderer);
    }

    cerrar();
    return 0;
}

bool inicializar() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << "Error SDL_Init: " << SDL_GetError() << endl;
        return false;
    }
    if (TTF_Init() == -1) {
        cout << "Error TTF_Init: " << TTF_GetError() << endl;
        return false;
    }
    if (IMG_Init(IMG_INIT_PNG) == 0) {
        cout << "Error IMG_Init: " << IMG_GetError() << endl;
        return false;
    }

    window = SDL_CreateWindow("Blackjack", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    font = TTF_OpenFont("fonts/RobotoMono-Bold.ttf", 22);
    if (!font) {
        cout << "No se pudo cargar la fuente: " << TTF_GetError() << endl;
        return false;
    }

    SDL_Surface* fondoSurface = IMG_Load("imagenes/fondo.png");
    if (!fondoSurface) {
        cout << "Error al cargar imagen de fondo: " << IMG_GetError() << endl;
        return false;
    }
    fondoTexture = SDL_CreateTextureFromSurface(renderer, fondoSurface);
    SDL_FreeSurface(fondoSurface);

    return true;
}

void cerrar() {
    SDL_DestroyTexture(fondoTexture);
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

bool estaEncima(SDL_Rect rect, int mouseX, int mouseY) {
    return (mouseX > rect.x && mouseX < rect.x + rect.w &&
            mouseY > rect.y && mouseY < rect.y + rect.h);
}

void renderTexto(const char* texto, SDL_Rect& rect, SDL_Color color, SDL_Renderer* renderer, TTF_Font* font) {
    SDL_Surface* surface = TTF_RenderText_Blended(font, texto, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    int textW, textH;
    SDL_QueryTexture(texture, NULL, NULL, &textW, &textH);
    SDL_Rect textRect = {
        rect.x + (rect.w - textW) / 2,
        rect.y + (rect.h - textH) / 2,
        textW, textH
    };
    SDL_RenderCopy(renderer, texture, NULL, &textRect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void dibujarBotonRedondeado(SDL_Renderer* renderer, SDL_Rect rect, SDL_Color color, const char* texto, SDL_Color colorTexto, TTF_Font* font) {
    roundedBoxRGBA(renderer, rect.x, rect.y, rect.x + rect.w, rect.y + rect.h, 10, color.r, color.g, color.b, color.a);
    renderTexto(texto, rect, colorTexto, renderer, font);
}



