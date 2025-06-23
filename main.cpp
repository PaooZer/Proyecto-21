#include <SDL2/SDL.h> // para manejar ventanas
#include <SDL_image.h> // cargar imagenes
#include <SDL_ttf.h> // incluir textos
#include <SDL2_gfxPrimitives.h> // dibujar botones redondos
#include <iostream>
#include <vector>
#include <string>
#include <sstream>  
#include<cstdlib>
#include<ctime>
#include<algorithm> //para swap
#include <fstream> //para archivos txt
#include <filesystem> //para listar todos los archivos .txt

using namespace std;
const int SCREEN_WIDTH = 800;
const int SCREEN_WEIGHT =600;

enum EstadoJuego {
    MENU_INICIAL,
    MENU_PRINCIPAL,
    NUEVA_PARTIDA,
    JUGANDO,
    CARGAR_PARTIDA,
    ACERCA_DE_NOSOTROS
};

struct Carta {
    string nombre;
    int valor;
    string rutaImagen;
};

struct juegoBlackJack {
    vector<Carta> mazo;
    vector<Carta> manoJugador;
    vector<Carta> manoDealer;
    EstadoJuego estado = MENU_INICIAL;
    string nombreJugador;
    bool turnoJugador = true; //controla si el jugador puede actuar
    string mensajeResultado; //mensaje de resultado
    int victorias = 0;
    int derrotas = 0;
    int empates = 0;
}juego;

struct partidaInfo {
    string nombre;
    int victorias;
    int derrotas;
    int empates;
    string fecha;
}part;


// ***** PROTOTIPOS *****

bool inicializar();
void cerrar();
bool estaEncima(SDL_Rect rect, int mouseX, int mouseY);
void renderTexto(const char* texto, SDL_Rect& rect, SDL_Color color, SDL_Renderer* renderer, TTF_Font* font);
void dibujarBotonRedondeado(SDL_Renderer* renderer, SDL_Rect rect, SDL_Color color, const char* texto, SDL_Color colorTexto, TTF_Font* font);
void mostrarAcercaDe(SDL_Renderer* renderer, TTF_Font* font, SDL_Rect botonVolver, int mouseX, int mouseY); //hay que progamarla y agregar boton de volver
void mostrarVentanaRegistro(SDL_Renderer* renderer, TTF_Font* font, SDL_Rect botonEmpezar, int mouseX, int mouseY, string& nombreJugador, bool& entradaActiva);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void iniciarNuevaPartida();  // inicializa todo para empezar una nueva ronda
void crearBaraja(vector<Carta>& mazo); // crea y devuelve las cartas
void barajearMazo(vector<Carta>& mazo); // barajea las cartas
void MostrarCartas(SDL_Renderer* renderer);
Carta DarCarta(vector<Carta>& baraja);   //da una carta de la baraja y la elimina del vector
int calcularPuntaje(const vector<Carta>& mano); // calcula el valor total de una mano
void manejarEventosJuego(int x, int y, SDL_Rect botonPedir, SDL_Rect botonPlantarse, SDL_Rect botonReiniciar, SDL_Rect botonSalir, EstadoJuego& estado);
void guardarPartida(const string& nombreJugador, int victorias, int derrotas, int empates); //para poder cargar las partidas (archivo txt)
bool cargarPartida(const string& nombreJugador, int& victorias, int& derrotas, int& empates, string& fecha); //cargar partida asociada al jugador
void mostrarVentanaCargar(SDL_Renderer* renderer, TTF_Font* font, SDL_Rect botonCargarJuego, int mouseX, int mouseY, string& nombreJugador, bool& entradaActiva, EstadoJuego& estado);
vector<partidaInfo> obtenerPartidasGuardadas();
string obtenerFechaActual();

// ***** VARIABLES DINAMICAS *****

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
TTF_Font* font = nullptr;
SDL_Texture* fondoTexture = nullptr;
SDL_Texture* cartaReverso = nullptr;



// ***** MAIN *****
int main(int argc, char* argv[]) {
    if (!inicializar()) return 1;

    bool quit = false;
    SDL_Rect botonEmpezar = { 300, 360, 200, 60 };
    SDL_Rect botonNueva   = { 300, 360, 200, 60 };
    SDL_Rect botonCargar  = { 300, 440, 200, 60 };
    SDL_Rect botonAcerca  = { 260, 520, 280, 60 };
    SDL_Rect botonVolver  = { 50, 520, 150, 50 };
    SDL_Rect botonEmpezarJuego = { SCREEN_WIDTH - 150 - 20, SCREEN_HEIGHT - 50 - 20, 150, 50 }; // Botón Empezar en la esquina inferior derecha
    SDL_Rect botonPedir = { SCREEN_WIDTH - 20 - 120, SCREEN_HEIGHT - 20 - 40 - 10 - 40, 120, 40 };
    SDL_Rect botonPlantarse = { SCREEN_WIDTH - 20 - 120, SCREEN_HEIGHT - 20 - 40, 120, 40 };
    SDL_Rect botonReiniciar = { SCREEN_WIDTH - 20 - 120, SCREEN_HEIGHT - 20 - 40 - 10 - 40, 120, 40 };
    SDL_Rect botonSalir = { SCREEN_WIDTH - 20 - 120, SCREEN_HEIGHT - 20 - 40, 120, 40 };
    SDL_Rect botonCargarJuego = { SCREEN_WIDTH - 150 - 20, SCREEN_HEIGHT - 50 - 20, 150, 50 }; 

    string nombreInput = "";
    bool entradaActiva = false;
    srand(time(NULL));

    EstadoJuego estado = MENU_INICIAL;
    bool quit = false; //para saber si el usuario cierra el programa
    
    while (!quit){
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        SDL_Event event;
        while (SDL_PollEvent(&event)){
            if (event.type == SDL_QUIT){
                quit = true;
            }

            if (event.type == SDL_MOUSEBUTTONDOWN){
                int x = event.button.x;
                int y = event.button.y;

                if (estado == MENU_INICIAL && estaEncima(botonEmpezar, x, y)){
                    estado = MENU_PRINCIPAL;
                }else if (estado == MENU_PRINCIPAL) {
                    if (estaEncima(botonNueva, x, y)){
                        estado = NUEVA_PARTIDA;
                        nombreInput = "";
                        entradaActiva = true;
                    }else if (estaEncima(botonCargar, x, y)) {
                        estado = CARGAR_PARTIDA;
                    }else if (estaEncima(botonAcerca, x, y)) {
                        estado = ACERCA_DE_NOSOTROS;
                    }
                }else if (estado == ACERCA_DE_NOSOTROS && estaEncima(botonVolver, x, y)){
                    estado = MENU_PRINCIPAL;
                }else if (estado == NUEVA_PARTIDA && estaEncima(botonEmpezarJuego, x, y)){
                    if (!nombreInput.empty()) {
                        juego.nombreJugador = nombreInput;
                        juego.victorias = 0;
                        juego.derrotas = 0;
                        juego.empates = 0;
                        iniciarNuevaPartida();
                        estado = JUGANDO;
                        entradaActiva = false;
                    }
                }else if (estado == CARGAR_PARTIDA && estaEncima(botonCargarJuego, x, y)){
                    if (!nombreInput.empty()) {
                        juego.nombreJugador = nombreInput;
                        if (cargarPartida(juego.nombreJugador, juego.victorias, juego.derrotas, juego.empates, part.fecha)){
                            iniciarNuevaPartida(); //iniciar partida con contadores cargados
                            estado = JUGANDO;
                        }else{
                            juego.mensajeResultado = "No se encontro la partida";
                            estado = MENU_PRINCIPAL;
                        }
                        entradaActiva = false;
                    }
                }else if (estado == JUGANDO){
                    manejarEventosJuego(x, y, botonPedir, botonPlantarse, botonReiniciar, botonSalir, estado);
                }
            }

            if ((estado == NUEVA_PARTIDA || estado == CARGAR_PARTIDA) && entradaActiva){
                if (event.type == SDL_TEXTINPUT){
                    nombreInput += event.text.text;
                }else if (event.type == SDL_KEYDOWN){
                    if (event.key.keysym.sym == SDLK_BACKSPACE && !nombreInput.empty()){
                        nombreInput.pop_back();
                    }else if (event.key.keysym.sym == SDLK_RETURN && !nombreInput.empty()){
                        juego.nombreJugador = nombreInput;
                        if (estado == NUEVA_PARTIDA){
                            juego.victorias = 0;
                            juego.derrotas = 0;
                            juego.empates = 0;
                            iniciarNuevaPartida();
                            estado = JUGANDO;
                        }else if (estado == CARGAR_PARTIDA){
                            if (cargarPartida(juego.nombreJugador, juego.victorias, juego.derrotas, juego.empates, part.fecha)) {
                                iniciarNuevaPartida();
                                estado = JUGANDO;
                            }else{
                                juego.mensajeResultado = "No se encontro la partida";
                                estado = MENU_PRINCIPAL;
                            }
                        }
                        entradaActiva = false;
                    }
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
        }else if (estado == MENU_PRINCIPAL){
            dibujarBotonRedondeado(renderer, botonNueva, estaEncima(botonNueva, mouseX, mouseY) ? SDL_Color{180, 180, 180, 255} : SDL_Color{100, 100, 100, 255}, "Nueva Partida", blanco, font);
            dibujarBotonRedondeado(renderer, botonCargar, estaEncima(botonCargar, mouseX, mouseY) ? SDL_Color{180, 180, 180, 255} : SDL_Color{100, 100, 100, 255}, "Cargar Partida", blanco, font);
            dibujarBotonRedondeado(renderer, botonAcerca, estaEncima(botonAcerca, mouseX, mouseY) ? SDL_Color{180, 180, 180, 255} : SDL_Color{100, 100, 100, 255}, "Acerca de Nosotras", blanco, font);
        }else if (estado == NUEVA_PARTIDA){
            mostrarVentanaRegistro(renderer, font, botonEmpezarJuego, mouseX, mouseY, nombreInput, entradaActiva);
        }else if (estado == CARGAR_PARTIDA){
            mostrarVentanaCargar(renderer, font, botonCargarJuego, mouseX, mouseY, nombreInput, entradaActiva, estado);
        }else if (estado == JUGANDO){
            MostrarCartas(renderer, botonPedir, botonPlantarse, botonReiniciar, botonSalir);
        }else if (estado == ACERCA_DE_NOSOTROS){
            mostrarAcercaDe(renderer, font, botonVolver, mouseX, mouseY);
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

    SDL_StartTextInput(); //habilita entrada de texto
    return true;
}

void cerrar(){
    SDL_StopTextInput();
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

void mostrarAcercaDe(SDL_Renderer* renderer, TTF_Font* font, SDL_Rect botonVolver, int mouseX, int mouseY) {

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    const char* titulo = "<3 Acerca de nosotros <3";
    SDL_Color colorTitulo = {255, 255, 255, 255};

    SDL_Surface* tituloSurface = TTF_RenderText_Blended(font, titulo, colorTitulo);
    SDL_Texture* tituloTexture = SDL_CreateTextureFromSurface(renderer, tituloSurface);


    int ventanaAncho = 800;
    int tituloX = (ventanaAncho - tituloSurface->w) / 2;
    SDL_Rect tituloRect = { tituloX, 30, tituloSurface->w, tituloSurface->h};
    SDL_RenderCopy(renderer, tituloTexture, NULL, &tituloRect);

    SDL_FreeSurface(tituloSurface);
    SDL_DestroyTexture(tituloTexture);

    SDL_Surface* collageSurface = IMG_Load("imagenes/chavas.png");
    if (collageSurface) {
        SDL_Texture* collageTexture = SDL_CreateTextureFromSurface(renderer, collageSurface);

        int imgW = collageSurface->w;
        int imgH = collageSurface->h;

        float scaleX = 800.0f / imgW;
        float scaleY = 600.0f / imgH;
        float scale = (scaleX < scaleY) ? scaleX : scaleY;

        int newW = imgW * scale;
        int newH = imgH * scale;

        SDL_Rect dstRect = {
            (800 - newW) / 2,
            (600 - newH) / 2,
            newW,
            newH
        };

        SDL_RenderCopy(renderer, collageTexture, NULL, &dstRect);

        SDL_FreeSurface(collageSurface);
        SDL_DestroyTexture(collageTexture);
    }

    SDL_Color volverColor = estaEncima(botonVolver, mouseX, mouseY) ? SDL_Color{180,180,180,255} : SDL_Color{100,100,100,255};
    dibujarBotonRedondeado(renderer, botonVolver, volverColor, "Volver", SDL_Color{255,255,255,255}, font);
}


void mostrarVentanaRegistro(SDL_Renderer* renderer, TTF_Font* font, SDL_Rect botonEmpezar, int mouseX, int mouseY, string& nombreJugador, bool& entradaActiva) {
    //fondo negro
    //cargar imagen de fondo
    SDL_Surface* fondoSurface = IMG_Load("imagenes/registro.png");
    if (fondoSurface){
        SDL_Texture* fondoRegistro = SDL_CreateTextureFromSurface(renderer, fondoSurface);
        if (fondoRegistro){
            int imgW = fondoSurface->w;
            int imgH = fondoSurface->h;

            //cubrir la ventana sin deformaciones
            float scaleX = static_cast<float>(SCREEN_WIDTH) / imgW;
            float scaleY = static_cast<float>(SCREEN_HEIGHT) / imgH;
            float scale = (scaleX > scaleY) ? scaleX : scaleY;

            int newW = static_cast<int>(imgW * scale);
            int newH = static_cast<int>(imgH * scale);

            //centrar la imagen
            SDL_Rect dstRect ={
                (SCREEN_WIDTH - newW)/2,
                (SCREEN_HEIGHT - newH)/2,
                newW,
                newH
            };

            SDL_RenderCopy(renderer, fondoRegistro, nullptr, &dstRect);
            SDL_DestroyTexture(fondoRegistro);
        }else{
            SDL_Log("No se pudo crear textura de fondo_registro.png: %s", SDL_GetError());
        }
        SDL_FreeSurface(fondoSurface);
    }else{
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_Log("No se pudo cargar fondo_registro.png: %s", IMG_GetError());
    }

    //titulo
    const char* titulo = "Ingresa tu nombre";
    SDL_Rect rectTitulo = { 0, 100, SCREEN_WIDTH, 50 };
    renderTexto(titulo, rectTitulo, {255, 255, 255}, renderer, font);

    //rectangulo que recibe el texto
    SDL_Rect rectInput = { 200, 250, 400, 50 };
    roundedBoxRGBA(renderer, rectInput.x, rectInput.y, rectInput.x + rectInput.w, rectInput.y + rectInput.h, 10, 50, 50, 50, 255);
    string textoInput = nombreJugador.empty() ? "Escribe aqui..." : nombreJugador;
    SDL_Color colorTexto = nombreJugador.empty() ? SDL_Color{150, 150, 150, 255} : SDL_Color{255, 255, 255, 255};
    renderTexto(textoInput.c_str(), rectInput, colorTexto, renderer, font);

    //boton Empezar
    SDL_Color colorBoton = estaEncima(botonEmpezar, mouseX, mouseY) ? SDL_Color{180, 180, 180, 255} : SDL_Color{100, 100, 100, 255};
    dibujarBotonRedondeado(renderer, botonEmpezar, colorBoton, "Empezar", {255, 255, 255}, font);
}

void crearBaraja(vector<Carta>& mazo){
    vector<string> palos = {"corazon", "picas", "trebol", "diamante"};
    vector<string> nombres = {"A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"};

    for (int i = 0; i < palos.size(); ++i){
        for (int j = 0; j < nombres.size(); ++j){
            Carta carta;
            carta.nombre = nombres[j];

            //asignar el valor de la carta
            if (nombres[j] == "A") {
                carta.valor = 11;
            } else if (nombres[j] == "J" || nombres[j] == "Q" || nombres[j] == "K") {
                carta.valor = 10;
            } else {
                carta.valor = stoi(nombres[j]); //convierte "2"-"10" en número
            }

            carta.rutaImagen = "imagenes/cartas/" + nombres[j] + "_" + palos[i] + ".png";

            mazo.push_back(carta); //agrega la carta c al final del vector "mazo"
        }
    }
}


void barajearMazo(vector<Carta>& mazo){
    int n;
    n = mazo.size();
    for (int i = n - 1; i > 0; --i){
        int j = rand() % (i + 1); // Numero aleatorio entre 0 e i
        swap(mazo[i], mazo[j]); // Para intercambiar las cartas de posicion
    }
}

void iniciarNuevaPartida(){
    juego.mazo.clear();
    crearBaraja(juego.mazo);
    barajearMazo(juego.mazo);
    //SDL_Log("Baraja creada y barajada. Tamaño: %zu", juego.mazo.size());

    juego.manoJugador.clear();
    juego.manoDealer.clear();
    //SDL_Log("Manos limpiadas. Mano dealer size: %zu, Mano jugador size: %zu", 
            //juego.manoDealer.size(), juego.manoJugador.size());

    juego.manoJugador.push_back(DarCarta(juego.mazo));
    juego.manoJugador.push_back(DarCarta(juego.mazo));
    juego.manoDealer.push_back(DarCarta(juego.mazo));
    juego.manoDealer.push_back(DarCarta(juego.mazo));
    //SDL_Log("Cartas repartidas. Mano jugador: %zu, Mano dealer: %zu, Mazo restante: %zu",
            //juego.manoJugador.size(), juego.manoDealer.size(), juego.mazo.size());
    //SDL_Log("Dealer carta 0: %s, carta 1: %s",
            //juego.manoDealer[0].rutaImagen.c_str(), juego.manoDealer[1].rutaImagen.c_str());
   // SDL_Log("Jugador carta 0: %s, carta 1: %s",
            //juego.manoJugador[0].rutaImagen.c_str(), juego.manoJugador[1].rutaImagen.c_str());

    juego.turnoJugador = true;
    juego.mensajeResultado = "";
    SDL_Log("Nueva partida iniciada. Turno jugador: %d", juego.turnoJugador);
}

void MostrarCartas(SDL_Renderer* renderer, SDL_Rect botonPedir, SDL_Rect botonPlantarse, SDL_Rect botonReiniciar, SDL_Rect botonSalir) {
    int espacio = 20;
    int anchoCarta = 100;
    int altoCarta = 150;

    // Fondo negro
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    //cartas del dealer
    int xInicialDealer = (SCREEN_WIDTH - (anchoCarta + espacio) * juego.manoDealer.size()) / 2;
    int yDealer = 50;

    for (size_t i = 0; i < juego.manoDealer.size(); ++i) {
        SDL_Rect rect = { xInicialDealer + static_cast<int>(i) * (anchoCarta + espacio), yDealer, anchoCarta, altoCarta };
        string ruta = (juego.turnoJugador && i == 1) ? "imagenes/cartas/1_Base.png" : juego.manoDealer[i].rutaImagen;
        SDL_Texture* textura = IMG_LoadTexture(renderer, ruta.c_str());
        if (textura) {
            SDL_RenderCopy(renderer, textura, nullptr, &rect);
            //SDL_Log("Carta dealer %zu: %s (renderizada)", i, ruta.c_str());
            SDL_DestroyTexture(textura);
        } else {
            SDL_Log("No se pudo cargar textura: %s", ruta.c_str());
        }
    }

    //cartas del jugador
    int xInicialJugador = (SCREEN_WIDTH - (anchoCarta + espacio) * juego.manoJugador.size()) / 2;
    int yJugador = SCREEN_HEIGHT - altoCarta - 50;

    for (size_t i = 0; i < juego.manoJugador.size(); ++i) {
        SDL_Rect rect = { xInicialJugador + static_cast<int>(i) * (anchoCarta + espacio), yJugador, anchoCarta, altoCarta };
        SDL_Texture* textura = IMG_LoadTexture(renderer, juego.manoJugador[i].rutaImagen.c_str());
        if (textura) {
            SDL_RenderCopy(renderer, textura, nullptr, &rect);
            SDL_DestroyTexture(textura);
        } else {
            SDL_Log("No se pudo cargar textura: %s", juego.manoJugador[i].rutaImagen.c_str());
        }
    }

    //mostrar puntajes
    SDL_Color blanco = { 255, 255, 255 };
    string puntajeJugador = "Puntaje: " + to_string(calcularPuntaje(juego.manoJugador));
    SDL_Rect rectPuntajeJugador = { 0, SCREEN_HEIGHT / 2 + 20, SCREEN_WIDTH, 30 };
    renderTexto(puntajeJugador.c_str(), rectPuntajeJugador, blanco, renderer, font);

    if (!juego.turnoJugador) {
        string puntajeDealer = "Dealer: " + to_string(calcularPuntaje(juego.manoDealer));
        SDL_Rect rectPuntajeDealer = { 0, 20, SCREEN_WIDTH, 30 };
        renderTexto(puntajeDealer.c_str(), rectPuntajeDealer, blanco, renderer, font);
    }

    //mostrar botones segun el estado
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    if (juego.turnoJugador) {
        SDL_Color colorPedir = estaEncima(botonPedir, mouseX, mouseY) ? SDL_Color{180, 180, 180, 255} : SDL_Color{100, 100, 100, 255};
        dibujarBotonRedondeado(renderer, botonPedir, colorPedir, "Pedir", blanco, font);
        SDL_Color colorPlantarse = estaEncima(botonPlantarse, mouseX, mouseY) ? SDL_Color{180, 180, 180, 255} : SDL_Color{100, 100, 100, 255};
        dibujarBotonRedondeado(renderer, botonPlantarse, colorPlantarse, "Plantarse", blanco, font);
    } else {
        SDL_Color colorReiniciar = estaEncima(botonReiniciar, mouseX, mouseY) ? SDL_Color{180, 180, 180, 255} : SDL_Color{100, 100, 100, 255};
        dibujarBotonRedondeado(renderer, botonReiniciar, colorReiniciar, "Reiniciar", blanco, font);
        SDL_Color colorSalir = estaEncima(botonSalir, mouseX, mouseY) ? SDL_Color{180, 180, 180, 255} : SDL_Color{100, 100, 100, 255};
        dibujarBotonRedondeado(renderer, botonSalir, colorSalir, "Salir", blanco, font);

        if (!juego.mensajeResultado.empty()) {
            SDL_Rect rectMensaje = { 0, SCREEN_HEIGHT / 2 - 20, SCREEN_WIDTH, 40 };
            renderTexto(juego.mensajeResultado.c_str(), rectMensaje, blanco, renderer, font);
        }
    }
}

Carta DarCarta(vector<Carta>& baraja) {
    if (baraja.empty()) {
        return Carta{"", 0, ""}; //por si no hay cartas que dar
    }

    Carta carta = baraja.back();  //obtiene la ultima carta del vector
    baraja.pop_back();            //la elimina del mazo
    return carta;                 //devuelve la carta para usarla
}

int calcularPuntaje(const vector<Carta>& mano){
    int puntaje = 0;
    int cantidadAses = 0;

    for (int i = 0; i < mano.size(); ++i){
        if (mano[i].nombre == "A") {
            cantidadAses++;
            puntaje += 11;  //Cuenta el  As como 11 inicialmente
        } else {
            puntaje += mano[i].valor;
        }
    }

    while (puntaje > 21 && cantidadAses > 0){
        puntaje -= 10;  //cambia un As de 11 a 1
        cantidadAses--;
    }

    return puntaje;
}


string obtenerFechaActual(){
    time_t ahora = time(nullptr);
    tm* tiempo = localtime(&ahora);
    char buffer[11];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", tiempo);
    return string(buffer);
}

void manejarEventosJuego(int x, int y, SDL_Rect botonPedir, SDL_Rect botonPlantarse, SDL_Rect botonReiniciar, SDL_Rect botonSalir, EstadoJuego& estado) {
    if (juego.turnoJugador) {
        if (estaEncima(botonPedir, x, y)) {
            juego.manoJugador.push_back(DarCarta(juego.mazo));
            SDL_Log("Jugador pide carta. Mano size: %zu", juego.manoJugador.size());
            if (calcularPuntaje(juego.manoJugador) > 21) {
                juego.turnoJugador = false;
                juego.mensajeResultado = "Te pasaste, Dealer gana :(";
                juego.derrotas++;
                guardarPartida(juego.nombreJugador, juego.victorias, juego.derrotas, juego.empates);
                SDL_Log("Jugador se pasa");
                MostrarCartas(renderer, botonPedir, botonPlantarse, botonReiniciar, botonSalir);
                SDL_RenderPresent(renderer);
            }
        } else if (estaEncima(botonPlantarse, x, y)) {
            juego.turnoJugador = false;
            SDL_Log("Jugador se planta");
            // SDL_Log("Dealer inicial: %zu cartas, Puntaje: %d", juego.manoDealer.size(), calcularPuntaje(juego.manoDealer));
            while (calcularPuntaje(juego.manoDealer) < 17) {
                juego.manoDealer.push_back(DarCarta(juego.mazo));
                SDL_Log("Dealer pide carta. Mano size: %zu", juego.manoDealer.size());
            }
            // SDL_Log("Dealer final: %zu cartas, Puntaje: %d", juego.manoDealer.size(), calcularPuntaje(juego.manoDealer));
            int puntajeJugador = calcularPuntaje(juego.manoJugador);
            int puntajeDealer = calcularPuntaje(juego.manoDealer);
            if (puntajeDealer > 21){
                juego.mensajeResultado = "Ganaste, Dealer se paso.";
                juego.victorias++;
            }else if (puntajeJugador > puntajeDealer){
                juego.mensajeResultado = "Ganaste";
                juego.victorias++;
            }else if (puntajeDealer > puntajeJugador){
                juego.mensajeResultado = "Dealer gana.";
                juego.derrotas++;
            }else{
                juego.mensajeResultado = "Empate.";
                juego.empates++;
                juego.derrotas++;
            }
            guardarPartida(juego.nombreJugador, juego.victorias, juego.derrotas, juego.empates);
            SDL_Log("Resultado: %s", juego.mensajeResultado.c_str());
            MostrarCartas(renderer, botonPedir, botonPlantarse, botonReiniciar, botonSalir);
            SDL_RenderPresent(renderer);
        }
    }else{
        if (estaEncima(botonReiniciar, x, y)){
            iniciarNuevaPartida();
            //SDL_Log("Partida reiniciada");
            MostrarCartas(renderer, botonPedir, botonPlantarse, botonReiniciar, botonSalir);
            SDL_RenderPresent(renderer);
        }else if (estaEncima(botonSalir, x, y)){
            estado = MENU_PRINCIPAL;
            juego.turnoJugador = true;
            juego.mensajeResultado = "";
           // SDL_Log("Saliendo a menu principal");
        }
    }
}

void mostrarVentanaCargar(SDL_Renderer* renderer, TTF_Font* font, SDL_Rect botonCargar, int mouseX, int mouseY, string& nombreJugador, bool& entradaActiva, EstadoJuego& estado) {
    SDL_Surface* fondoSurface = IMG_Load("imagenes/cargar.png");
    if (fondoSurface){
        SDL_Texture* fondoRegistro = SDL_CreateTextureFromSurface(renderer, fondoSurface);
        if (fondoRegistro) {
            int imgW = fondoSurface->w;
            int imgH = fondoSurface->h;
            float scaleX = static_cast<float>(SCREEN_WIDTH) / imgW;
            float scaleY = static_cast<float>(SCREEN_HEIGHT) / imgH;
            float scale = (scaleX > scaleY) ? scaleX : scaleY;
            int newW = static_cast<int>(imgW * scale);
            int newH = static_cast<int>(imgH * scale);
            SDL_Rect dstRect = {
                (SCREEN_WIDTH - newW)/2,
                (SCREEN_HEIGHT - newH)/2,
                newW,
                newH
            };
            SDL_RenderCopy(renderer, fondoRegistro, nullptr, &dstRect);
            SDL_DestroyTexture(fondoRegistro);
        }else{
            SDL_Log("No se pudo crear textura de fondo_cargar.png: %s", SDL_GetError());
        }
        SDL_FreeSurface(fondoSurface);
    }else{
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_Log("No se pudo cargar fondo_cargar.png: %s", IMG_GetError());
    }

    const char* titulo = "Selecciona una partida";
    SDL_Rect rectTitulo = {0, 50, SCREEN_WIDTH, 50};
    renderTexto(titulo, rectTitulo, {255, 255, 255}, renderer, font);

    vector<partidaInfo> partidas = obtenerPartidasGuardadas();
    SDL_Rect botonVolver = {50, SCREEN_HEIGHT - 70, 150, 50};
    SDL_Color blanco = {255, 255, 255};

    if (partidas.empty()){
        SDL_Rect rectMensaje = {0, 200, SCREEN_WIDTH, 50};
        renderTexto("No hay partidas guardadas", rectMensaje, blanco, renderer, font);
    }else{
        int yInicial = 150;
        int altoBoton = 60;
        int espacio = 10;
        static vector<SDL_Rect> botonesPartidas;
        botonesPartidas.clear();

        for (size_t i = 0; i < partidas.size(); ++i){
            SDL_Rect rectBoton = {200, static_cast<int>(yInicial + i * (altoBoton + espacio)), 400, altoBoton};
            botonesPartidas.push_back(rectBoton);

            string texto = partidas[i].nombre + " (G: " + to_string(partidas[i].victorias) + 
                          " P: " + to_string(partidas[i].derrotas) + " E: " + to_string(partidas[i].empates) + 
                          " - " + partidas[i].fecha + ")";
            SDL_Color colorBoton = estaEncima(rectBoton, mouseX, mouseY) ? SDL_Color{180, 180, 180, 255} : SDL_Color{100, 100, 100, 255};
            dibujarBotonRedondeado(renderer, rectBoton, colorBoton, texto.c_str(), blanco, font);

            if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)){
                if (estaEncima(rectBoton, mouseX, mouseY)){
                    juego.nombreJugador = partidas[i].nombre;
                    string fecha;
                    if (cargarPartida(juego.nombreJugador, juego.victorias, juego.derrotas, juego.empates, fecha)) {
                        iniciarNuevaPartida();
                        estado = JUGANDO;
                    }
                }
            }
        }
    }

    SDL_Color colorVolver = estaEncima(botonVolver, mouseX, mouseY) ? SDL_Color{180, 180, 180, 255} : SDL_Color{100, 100, 100, 255};
    dibujarBotonRedondeado(renderer, botonVolver, colorVolver, "Volver", blanco, font);

    // Manejar clic en "Volver"
    if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)){
        if (estaEncima(botonVolver, mouseX, mouseY)){
            estado = MENU_PRINCIPAL;
        }
    }
}

void guardarPartida(const string& nombreJugador, int victorias, int derrotas, int empates) {
    ofstream archivo; //crear archivo 
    archivo.open("partidas/" + nombreJugador + ".txt", ios::out);
    if (archivo.is_open()){ //verifica que el archivo existe y se abrio correctamente.
        archivo << obtenerFechaActual() << "\n";
        archivo << victorias << "\n" << derrotas << "\n" << empates << "\n";
        archivo.close();
        SDL_Log("Partida guardada en partidas/%s.txt", nombreJugador.c_str());
    } else {
        SDL_Log("Error al guardar partida en partidas/%s.txt", nombreJugador.c_str());
    }
}

bool cargarPartida(const string& nombreJugador, int& victorias, int& derrotas, int& empates, string& fecha){
    ifstream archivo; //para leer 
    archivo.open("partidas/" + nombreJugador + ".txt", ios::in);
    if (archivo.is_open()){ //verifica que el archivo existe y se abrio correctamente.
        string primeraLinea;
        getline(archivo, primeraLinea);
       if (primeraLinea.length() == 10 && primeraLinea[4] == '-' && primeraLinea[7] == '-'){
            fecha = primeraLinea;
            if (archivo >> victorias >> derrotas >> empates){
                archivo.close();
                return true;
            }else{
                archivo.close();
                SDL_Log("Formato invalido en partidas/%s.txt", nombreJugador.c_str());
                return false;
            }
        }else{
            fecha = "Sin fecha";
            stringstream ss(primeraLinea + "\n" + string(istreambuf_iterator<char>(archivo), {}));
            if (ss >> victorias >> derrotas >> empates){
                archivo.close();
                return true;
            }else{
                archivo.close();
                SDL_Log("Formato invalido en partidas/%s.txt", nombreJugador.c_str());
                return false;
            }
        }
    }else{
        SDL_Log("No se encontro partidas/%s.txt", nombreJugador.c_str());
        return false;
    }
}

vector<partidaInfo> obtenerPartidasGuardadas(){
    vector<partidaInfo> partidas;
    for (const auto& entry : filesystem::directory_iterator("partidas")){
        if (entry.path().extension() == ".txt") {
            string nombre = entry.path().stem().string();
            int victorias, derrotas, empates;
            string fecha;
            if (cargarPartida(nombre, victorias, derrotas, empates, fecha)){
                partidas.push_back({nombre, victorias, derrotas, empates, fecha});
            }
        }
    }
    return partidas;
}


