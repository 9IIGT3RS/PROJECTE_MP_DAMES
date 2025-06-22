#include "../include/joc.hpp"
#include "../include/GraphicManager.h"
#include <iostream>

// Constructor
Joc::Joc()
    : m_modeJoc(MODE_JOC_NONE)
    , m_jugadorActual(COLOR_BLANC)
    , m_partidaAcabada(false)
    , m_guanyador(COLOR_BLANC)
    , m_fitxaSeleccionada(false)
    , m_mouseAnterior(false)
    , m_tempsUltimMoviment(0)
    , m_fitxesBlanquesMortes(0)
    , m_fitxesNegresMortes(0)
{
}

// Inicialitza la partida segons el mode
void Joc::inicialitza(ModeJoc mode, const std::string& nomFitxerTauler, const std::string& nomFitxerMoviments) {
    m_modeJoc = mode;
    m_fitxerTauler = nomFitxerTauler;
    m_fitxerMoviments = nomFitxerMoviments;

    // Inicialitzem el tauler sempre
    if (!m_tauler.carregaPartidaDesArxiu(nomFitxerTauler)) {
        std::cout << "Error: No s'ha pogut carregar el fitxer del tauler: " << nomFitxerTauler << std::endl;
        return;
    }

    // Actualitzem moviments inicials
    m_tauler.actualitzaMovimentsValids();

    // Inicialitzem segons el mode
    switch (mode) {
    case MODE_JOC_NORMAL:
        inicialitzaModusNormal();
        break;
    case MODE_JOC_REPLAY:
        inicialitzaModusReplay();
        break;
    default:
        std::cout << "Error: Mode de joc no vàlid" << std::endl;
        break;
    }

    std::cout << "Partida inicialitzada en mode: " << getStringModeJoc() << std::endl;
}

// Mètode principal que es crida cada cicle
bool Joc::actualitza(int mousePosX, int mousePosY, bool mouseStatus) {
    // Sempre dibuixem el fons i el tauler
    dibuixaFons();
    dibuixaTauler();
    dibuixaFitxes();

    // Si hi ha una fitxa seleccionada, mostrem les posicions possibles
    if (m_fitxaSeleccionada) {
        dibuixaPosicionsPossibles();
    }

    // Dibuixem la informació de la partida
    dibuixaInformacioPartida();

    // Si la partida ja ha acabat, només mostrem el resultat
    if (m_partidaAcabada) {
        dibuixaPantallaFinal();
        return true;
    }

    // Actualitzem segons el mode
    bool partidaAcabada = false;
    switch (m_modeJoc) {
    case MODE_JOC_NORMAL:
        partidaAcabada = actualitzaModusNormal(mousePosX, mousePosY, mouseStatus);
        break;
    case MODE_JOC_REPLAY:
        partidaAcabada = actualitzaModusReplay(mousePosX, mousePosY, mouseStatus);
        break;
    default:
        break;
    }

    return partidaAcabada;
}

// Finalitza la partida
void Joc::finalitza() {
    if (m_modeJoc == MODE_JOC_NORMAL) {
        // Guardem els moviments al fitxer
        if (m_cua.guardaMovimentsAFitxer(m_fitxerMoviments)) {
            std::cout << "Moviments guardats correctament a: " << m_fitxerMoviments << std::endl;
        }
        else {
            std::cout << "Error guardant moviments al fitxer: " << m_fitxerMoviments << std::endl;
        }
    }

    std::cout << "Partida finalitzada." << std::endl;
}

// ========== MÈTODES PRIVATS ==========

// Inicialitza mode normal
void Joc::inicialitzaModusNormal() {
    m_jugadorActual = COLOR_BLANC;
    m_partidaAcabada = false;
    m_fitxaSeleccionada = false;
    m_posicionsValides.clear();
    m_mouseAnterior = false;
}

// Inicialitza mode replay
void Joc::inicialitzaModusReplay() {
    m_jugadorActual = COLOR_BLANC;
    m_partidaAcabada = false;
    m_fitxaSeleccionada = false;
    m_posicionsValides.clear();
    m_tempsUltimMoviment = 0;
    m_mouseAnterior = false;

    // Carreguem els moviments del fitxer
    if (!m_cua.carregaMovimentsDeFitxer(m_fitxerMoviments)) {
        std::cout << "Error: No s'ha pogut carregar el fitxer de moviments: " << m_fitxerMoviments << std::endl;
        m_partidaAcabada = true;
    }
    else {
        std::cout << "Fitxer de moviments carregat correctament." << std::endl;
    }
}

// Actualitza mode normal amb interacció real del mouse
bool Joc::actualitzaModusNormal(int mousePosX, int mousePosY, bool mouseStatus) {
    // Detectem click del mouse (flanc positiu)
    bool mouseClick = mouseStatus && !m_mouseAnterior;
    m_mouseAnterior = mouseStatus;

    if (mouseClick && estaDinsLimitsTauler(mousePosX, mousePosY)) {
        Posicio posClick = converteixCoordenadesMouseATauler(mousePosX, mousePosY);

        if (!m_fitxaSeleccionada) {
            // Intentem seleccionar una fitxa
            if (esFitxaDelJugadorActual(posClick)) {
                seleccionaFitxa(posClick);
            }
        }
        else {
            // Ja tenim una fitxa seleccionada
            if (posClick == m_posicioSeleccionada) {
                // Deseleccionem la fitxa
                m_fitxaSeleccionada = false;
                m_posicionsValides.clear();
            }
            else if (esPosicioValida(posClick)) {
                // Executem el moviment
                executaMoviment(posClick);

                // Comprovem si la partida ha acabat
                if (comprovaFinalPartida()) {
                    m_partidaAcabada = true;
                    m_guanyador = getGuanyador();
                    return true;
                }
            }
            else if (esFitxaDelJugadorActual(posClick)) {
                // Seleccionem una altra fitxa
                seleccionaFitxa(posClick);
            }
            else {
                // Click en posició no vàlida, deseleccionem
                m_fitxaSeleccionada = false;
                m_posicionsValides.clear();
            }
        }
    }

    return false;
}


bool Joc::actualitzaModusReplay(int mousePosX, int mousePosY, bool mouseStatus) {
    // detecta click
    bool mouseClick = mouseStatus && !m_mouseAnterior;
    m_mouseAnterior = mouseStatus;

    if (mouseClick) {
        if (!m_cua.esBuida()) {
            // OBTENER PRÓXIMO MOVIMIENTO Y ELIMINARLO
            Moviment moviment = m_cua.obteSeguentMoviment();

            if (!moviment.esMovimentValid()) {
                std::cout << "Error: Moviment no valid en replay." << std::endl;
                return false;
            }

            Posicio origen = moviment.getPosicioInicial();
            Posicio desti = moviment.getPosicioFinal();

            // CONFIGURAR per utilitzar executaMoviment()
            m_fitxaSeleccionada = true;
            m_posicioSeleccionada = origen;
            executaMoviment(desti);

            std::cout << "Moviment reproduït: " << origen << " -> " << desti << std::endl;

            // Verificar fin de partida
            if (comprovaFinalPartida()) {
                m_partidaAcabada = true;
                m_guanyador = getGuanyador();
                return true;
            }
        }
        else {
            // NO HAY MÁS MOVIMIENTOS
            std::cout << "No queden mes moviments a reproduir." << std::endl;

          
            m_partidaAcabada = true;
            return true;
        }
    }

    return false;
}

// ========== MÈTODES DE DIBUIX ==========

void Joc::dibuixaFons() {
    GraphicManager::getInstance()->drawSprite(GRAFIC_FONS, 0, 0);
}

void Joc::dibuixaTauler() {
    GraphicManager::getInstance()->drawSprite(GRAFIC_TAULER, POS_X_TAULER, POS_Y_TAULER);
}

void Joc::dibuixaFitxes() {
    for (int fila = 0; fila < N_FILES; fila++) {
        for (int col = 0; col < N_COLUMNES; col++) {
            const Fitxa& fitxa = m_tauler.getFitxa(fila, col);

            if (fitxa.esFitxaJugador()) {
                float posX = static_cast<float>(POS_X_TAULER + CASELLA_INICIAL_X + col * AMPLADA_CASELLA);
                float posY = static_cast<float>(POS_Y_TAULER + CASELLA_INICIAL_Y + (N_FILES - 1 - fila) * ALCADA_CASELLA);

                IMAGE_NAME imatge = obteImatgeFitxa(fitxa);
                GraphicManager::getInstance()->drawSprite(imatge, posX, posY);
            }
        }
    }
}

void Joc::dibuixaPosicionsPossibles() {
    for (const Posicio& pos : m_posicionsValides) {
        float posX = static_cast<float>(POS_X_TAULER + CASELLA_INICIAL_X + pos.getColumna() * AMPLADA_CASELLA);
        float posY = static_cast<float>(POS_Y_TAULER + CASELLA_INICIAL_Y + (N_FILES - 1 - pos.getFila()) * ALCADA_CASELLA);

        GraphicManager::getInstance()->drawSprite(GRAFIC_POSICIO_VALIDA, posX, posY);
    }
}

void Joc::dibuixaInformacioPartida() {
    GraphicManager* gm = GraphicManager::getInstance();

    // Títol del mode
    std::string titol = "MODE: " + getStringModeJoc();
    gm->drawFont(FONT_WHITE_30, 270, 610, 1.0f, titol);

    if (!m_partidaAcabada) {

        std::string torn = "TORN: ";
        gm->drawFont(FONT_WHITE_30, 550, 680, 0.8f, torn);

        // Dibuixem la fitxa del jugador actual
        IMAGE_NAME fitxaTorn = (m_jugadorActual == COLOR_BLANC) ? GRAFIC_FITXA_BLANCA : GRAFIC_FITXA_NEGRA;
        gm->drawSprite(fitxaTorn, 650, 650);

        // Text del color
        FONT_NAME fontColor = (m_jugadorActual == COLOR_BLANC) ? FONT_WHITE_30 : FONT_RED_30;
        gm->drawFont(fontColor, 550, 680, 1.0f, getStringColor(m_jugadorActual));

        // Fitxes capturades
        gm->drawFont(FONT_WHITE_30, 50, 650, 0.8f, "CAPTURES:");

        // Fitxes blanques capturades
        gm->drawFont(FONT_WHITE_30, 50, 680, 0.7f, "Blanques: " + std::to_string(m_fitxesBlanquesMortes));
        for (int i = 0; i < m_fitxesBlanquesMortes && i < 12; i++) {
            float x = 200.0f + (i % 6) * 30.0f;
            float y = 670.0f + (i / 6) * 30.0f;
            gm->drawSprite(GRAFIC_FITXA_BLANCA, x, y);
        }

        // Fitxes negres capturades
        gm->drawFont(FONT_RED_30, 50, 730, 0.7f, "Negres: " + std::to_string(m_fitxesNegresMortes));
        for (int i = 0; i < m_fitxesNegresMortes && i < 12; i++) {
            float x = 200.0f + (i % 6) * 30.0f;
            float y = 720.0f + (i / 6) * 30.0f;
            gm->drawSprite(GRAFIC_FITXA_NEGRA, x, y);
        }

        // Si hi ha captures obligatòries, ho indiquem
        if (m_tauler.hiHaCapturaPossible(m_jugadorActual)) {
            gm->drawFont(FONT_GREEN_30, 400, 650, 0.8f, "CAPTURA OBLIGATORIA!");
        }
    }
}

void Joc::dibuixaPantallaFinal() {
    GraphicManager* gm = GraphicManager::getInstance();

    gm->drawFont(FONT_GREEN_30, 200, 350, 1.2f, "PARTIDA ACABADA!");

    std::string guanyador = "GUANYADOR: " + getStringColor(m_guanyador);
    FONT_NAME fontColor = (m_guanyador == COLOR_BLANC) ? FONT_WHITE_30 : FONT_RED_30;
    gm->drawFont(fontColor, 200, 450, 1.0f, guanyador);
}

// ========== MÈTODES AUXILIARS ==========

// Converteix coordenades del mouse a posició del tauler
Posicio Joc::converteixCoordenadesMouseATauler(int mousePosX, int mousePosY) {
    int col = (mousePosX - POS_X_TAULER - CASELLA_INICIAL_X) / AMPLADA_CASELLA;
    int fila = N_FILES - 1 - ((mousePosY - POS_Y_TAULER - CASELLA_INICIAL_Y) / ALCADA_CASELLA);

    return Posicio(fila, col);
}

// Comprova si les coordenades estan dins del tauler
bool Joc::estaDinsLimitsTauler(int mousePosX, int mousePosY) {
    int minX = POS_X_TAULER + CASELLA_INICIAL_X;
    int maxX = minX + N_COLUMNES * AMPLADA_CASELLA;
    int minY = POS_Y_TAULER + CASELLA_INICIAL_Y;
    int maxY = minY + N_FILES * ALCADA_CASELLA;

    return (mousePosX >= minX && mousePosX < maxX &&
        mousePosY >= minY && mousePosY < maxY);
}

// Selecciona una fitxa i calcula els moviments possibles
void Joc::seleccionaFitxa(const Posicio& pos) {
    m_fitxaSeleccionada = true;
    m_posicioSeleccionada = pos;
    m_posicionsValides.clear();

    // Obtenim les posicions possibles
    Posicio posicionsPossibles[MAX_MOVIMENTS_POSSIBLES];
    int numPosicions = 0;

    m_tauler.getPosicionsPossibles(pos, numPosicions, posicionsPossibles);

    // Les guardem al vector
    for (int i = 0; i < numPosicions; i++) {
        m_posicionsValides.push_back(posicionsPossibles[i]);
    }
}

//Executar movimet
void Joc::executaMoviment(const Posicio& desti) {
    // Obtenim el moviment complet per saber les captures
    const Fitxa& fitxa = m_tauler.getFitxa(m_posicioSeleccionada);
    const std::vector<Moviment>& moviments = fitxa.getMoviments();
    Moviment movimentExecutat;
    for (const Moviment& mov : moviments) {
        if (mov.getPosicioFinal() == desti) {
            movimentExecutat = mov;
            break;
        }
    }

    // IMPORTANT: Obtenim la millor captura ABANS de moure la fitxa
    // perquè després del moviment el tauler ja haurà canviat
    Moviment millorCaptura;
    bool hiHaviaCapturesPossibles = m_tauler.hiHaCapturaPossible(m_jugadorActual);
    if (hiHaviaCapturesPossibles) {
        millorCaptura = m_tauler.obteMillorCaptura(m_jugadorActual);
    }

    // Actualitzem el comptador de captures
    const std::vector<Posicio>& captures = movimentExecutat.getCaptures();
    for (const Posicio& captura : captures) {
        const Fitxa& fitxaCapturada = m_tauler.getFitxa(captura);
        if (fitxaCapturada.getColor() == COLOR_BLANC) {
            m_fitxesBlanquesMortes++;
        }
        else {
            m_fitxesNegresMortes++;
        }
    }

    // Executem el moviment
    if (m_tauler.mouFitxa(m_posicioSeleccionada, desti)) {
        // Guardem el moviment
        m_cua.afegeixMoviment(movimentExecutat);

        // Apliquem la regla de bufat
        if (hiHaviaCapturesPossibles) {
            // Comprovem dos casos de bufat:
            // 1. No s'ha fet cap captura quan era obligatori
            // 2. S'ha fet una captura però no la màxima possible
            bool aplicarBufat = false;

            if (movimentExecutat.getNumCaptures() == 0) {
                // Cas 1: No s'ha capturat res quan era obligatori
                std::cout << "BUFAT! No has fet cap captura quan era obligatori!" << std::endl;
                aplicarBufat = true;
            }
            else if (millorCaptura.esMovimentValid() &&
                millorCaptura.getNumCaptures() > movimentExecutat.getNumCaptures()) {
                // Cas 2: S'ha capturat però no el màxim possible
                std::cout << "BUFAT! No has fet la captura maxima!" << std::endl;
                std::cout << "Captures possibles: " << millorCaptura.getNumCaptures()
                    << ", captures fetes: " << movimentExecutat.getNumCaptures() << std::endl;
                aplicarBufat = true;
            }

            if (aplicarBufat) {
                // Eliminem la fitxa que acabem de moure
                m_tauler.eliminaFitxa(desti);

                // Actualitzem el comptador
                if (m_jugadorActual == COLOR_BLANC) {
                    m_fitxesBlanquesMortes++;
                }
                else {
                    m_fitxesNegresMortes++;
                }

                // Actualitzem moviments després del bufat
                m_tauler.actualitzaMovimentsValids();
            }
        }

        // Canviem el torn
        canviaTorn();

        // Deseleccionem
        m_fitxaSeleccionada = false;
        m_posicionsValides.clear();
    }
}
// Comprova si una posició està entre les vàlides
bool Joc::esPosicioValida(const Posicio& pos) {
    for (const Posicio& posValida : m_posicionsValides) {
        if (pos == posValida) {
            return true;
        }
    }
    return false;
}

// Canvia el torn al següent jugador
void Joc::canviaTorn() {
    m_jugadorActual = (m_jugadorActual == COLOR_BLANC) ? COLOR_NEGRE : COLOR_BLANC;
    m_tauler.actualitzaMovimentsValids();
}

// Comprova si la partida ha acabat
bool Joc::comprovaFinalPartida() {
    int fitxesBlanques = 0, fitxesNegres = 0;
    bool potMoureBlanques = false, potMoureNegres = false;

    for (int fila = 0; fila < N_FILES; fila++) {
        for (int col = 0; col < N_COLUMNES; col++) {
            const Fitxa& fitxa = m_tauler.getFitxa(fila, col);
            if (fitxa.esFitxaJugador()) {
                if (fitxa.getColor() == COLOR_BLANC) {
                    fitxesBlanques++;
                    if (!fitxa.getMoviments().empty()) {
                        potMoureBlanques = true;
                    }
                }
                else {
                    fitxesNegres++;
                    if (!fitxa.getMoviments().empty()) {
                        potMoureNegres = true;
                    }
                }
            }
        }
    }

    // La partida acaba si un jugador no té fitxes o no pot moure
    return (fitxesBlanques == 0 || fitxesNegres == 0 ||
        (m_jugadorActual == COLOR_BLANC && !potMoureBlanques) ||
        (m_jugadorActual == COLOR_NEGRE && !potMoureNegres));
}

// Obté el guanyador
ColorFitxa Joc::getGuanyador() {
    int fitxesBlanques = 0, fitxesNegres = 0;

    for (int fila = 0; fila < N_FILES; fila++) {
        for (int col = 0; col < N_COLUMNES; col++) {
            const Fitxa& fitxa = m_tauler.getFitxa(fila, col);
            if (fitxa.esFitxaJugador()) {
                if (fitxa.getColor() == COLOR_BLANC) fitxesBlanques++;
                else fitxesNegres++;
            }
        }
    }

    if (fitxesBlanques == 0) return COLOR_NEGRE;
    if (fitxesNegres == 0) return COLOR_BLANC;

    // Si algú no pot moure, perd
    return (m_jugadorActual == COLOR_BLANC) ? COLOR_NEGRE : COLOR_BLANC;
}

// Comprova si una fitxa és del jugador actual
bool Joc::esFitxaDelJugadorActual(const Posicio& pos) {
    const Fitxa& fitxa = m_tauler.getFitxa(pos);
    return fitxa.esFitxaJugador() && fitxa.getColor() == m_jugadorActual;
}

// Obté la imatge corresponent a una fitxa
IMAGE_NAME Joc::obteImatgeFitxa(const Fitxa& fitxa) {
    if (fitxa.getColor() == COLOR_BLANC) {
        return (fitxa.getTipus() == FITXA_NORMAL) ? GRAFIC_FITXA_BLANCA : GRAFIC_DAMA_BLANCA;
    }
    else {
        return (fitxa.getTipus() == FITXA_NORMAL) ? GRAFIC_FITXA_NEGRA : GRAFIC_DAMA_NEGRA;
    }
}

// Converteix color a string
std::string Joc::getStringColor(ColorFitxa color) {
    return (color == COLOR_BLANC) ? "BLANQUES" : "NEGRES";
}

// Converteix mode a string
std::string Joc::getStringModeJoc() {
    switch (m_modeJoc) {
    case MODE_JOC_NORMAL: return "NORMAL";
    case MODE_JOC_REPLAY: return "REPLAY";
    default: return "DESCONEGUT";
    }
}
