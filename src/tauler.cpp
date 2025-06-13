#include "../include/tauler.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>

// Constructor
Tauler::Tauler() {
    inicialitzaTaulerBuit();
}

// Inicialització
void Tauler::inicialitzaTaulerBuit() {
    for (int fila = 0; fila < N_FILES; fila++) {
        for (int col = 0; col < N_COLUMNES; col++) {
            m_caselles[fila][col] = Fitxa();
            m_caselles[fila][col].setUbicacio(Posicio(fila, col));
        }
    }
}

bool Tauler::carregaPartidaDesArxiu(const std::string & nomArxiu) {
    inicialitzaTaulerBuit();

    std::ifstream arxiu(nomArxiu);
    if (!arxiu.is_open()) {
        std::cout << "Error: No s'ha pogut obrir el fitxer: " << nomArxiu << std::endl;
        return false;
    }

    Fitxa fitxa;
    while (arxiu >> fitxa) {
        colocaFitxa(fitxa);
    }

    arxiu.close();
    std::cout << "Tauler carregat des de: " << nomArxiu << std::endl;
    return true;
}

void Tauler::colocaFitxa(const Fitxa & fitxa) {
    Posicio pos = fitxa.getUbicacio();
    if (esPosicioValida(pos)) {
        m_caselles[pos.getFila()][pos.getColumna()] = fitxa;
    }
}

// Getters
const Fitxa& Tauler::getFitxa(const Posicio & pos) const {
    return m_caselles[pos.getFila()][pos.getColumna()];
}

const Fitxa& Tauler::getFitxa(int fila, int columna) const {
    return m_caselles[fila][columna];
}

bool Tauler::esCasellaLliure(const Posicio & pos) const {
    return esPosicioValida(pos) && getFitxa(pos).esBuida();
}

// Validacions
bool Tauler::esPosicioValida(const Posicio & pos) const {
    return pos.esValida();
}

bool Tauler::esMovimentValid(const Posicio & origen, const Posicio & desti) const {
    if (!esPosicioValida(origen) || !esPosicioValida(desti)) {
        return false;
    }

    const Fitxa& fitxa = getFitxa(origen);
    if (fitxa.esBuida()) {
        return false;
    }

    // Comprovem si el destí està entre els moviments vàlids
    const std::vector<Moviment>& moviments = fitxa.getMoviments();
    for (const Moviment& mov : moviments) {
        if (mov.getPosicioFinal() == desti) {
            return true;
        }
    }

    return false;
}

// Representació del tauler
std::string Tauler::toString() const {
    std::ostringstream oss;

    for (int fila = N_FILES - 1; fila >= 0; fila--) {
        oss << (fila + 1) << ": ";
        for (int col = 0; col < N_COLUMNES; col++) {
            oss << m_caselles[fila][col].getCaracter() << " ";
        }
        oss << "\n";
    }
    oss << "   a b c d e f g h";

    return oss.str();
}

// ========== ACTUALITZACIÓ DE MOVIMENTS ==========

void Tauler::actualitzaMovimentsValids() {
    // Actualitzem TOTS els moviments de TOTES les fitxes
    for (int fila = 0; fila < N_FILES; fila++) {
        for (int col = 0; col < N_COLUMNES; col++) {
            Fitxa& fitxa = m_caselles[fila][col];
            if (fitxa.esFitxaJugador()) {
                calculaMovimentsFitxa(fitxa);
            }
        }
    }
}

void Tauler::calculaMovimentsFitxa(Fitxa & fitxa) {
    fitxa.netejaMoviments();

    Posicio pos = fitxa.getUbicacio();
    ColorFitxa color = fitxa.getColor();
    TipusFitxa tipus = fitxa.getTipus();

    std::vector<Moviment> moviments;

    // Sempre busquem captures (obligatòries o no)
    cercaCaptures(pos, color, tipus, moviments);

    // Sempre busquem moviments simples també
    cercaMovimentsSimples(pos, color, tipus, moviments);

    // Assignem TOTS els moviments a la fitxa
    for (const Moviment& mov : moviments) {
        fitxa.afegeixMoviment(mov);
    }
}

void Tauler::getPosicionsPossibles(const Posicio & origen, int& numPosicions, Posicio posicionsPossibles[]) {
    numPosicions = 0;

    if (!esPosicioValida(origen)) {
        return;
    }

    const Fitxa& fitxa = getFitxa(origen);
    if (fitxa.esBuida()) {
        return;
    }

    const std::vector<Moviment>& moviments = fitxa.getMoviments();
    for (size_t i = 0; i < moviments.size() && numPosicions < MAX_MOVIMENTS_POSSIBLES; i++) {
        posicionsPossibles[numPosicions] = moviments[i].getPosicioFinal();
        numPosicions++;
    }
}

// ========== MOVIMENTS SIMPLES ==========

void Tauler::cercaMovimentsSimples(const Posicio & pos, ColorFitxa color, TipusFitxa tipus, std::vector<Moviment>&moviments) {
    if (tipus == FITXA_NORMAL) {
        calculaMovimentsNormals(pos, color, moviments);
    }
    else if (tipus == FITXA_DAMA) {
        calculaMovimentsDama(pos, color, moviments);
    }
}

void Tauler::calculaMovimentsNormals(const Posicio & pos, ColorFitxa color, std::vector<Moviment>&moviments) {
    int direccions[4][2] = { {-1, -1}, {-1, 1}, {1, -1}, {1, 1} };

    for (int i = 0; i < 4; i++) {
        int deltaFila = direccions[i][0];
        int deltaCol = direccions[i][1];

        // Les fitxes normals només es mouen cap endavant
        if (!esDireccioValidaPerNormal(deltaFila, color)) {
            continue;
        }

        Posicio desti = pos.getVei(deltaFila, deltaCol);
        if (esCasellaLliure(desti)) {
            Moviment mov(pos);
            mov.afegeixPosicio(desti);
            moviments.push_back(mov);
        }
    }
}

void Tauler::calculaMovimentsDama(const Posicio & pos, ColorFitxa color, std::vector<Moviment>&moviments) {
    int direccions[4][2] = { {-1, -1}, {-1, 1}, {1, -1}, {1, 1} };

    for (int i = 0; i < 4; i++) {
        int deltaFila = direccions[i][0];
        int deltaCol = direccions[i][1];

        // Les dames poden moure's múltiples caselles
        for (int distancia = 1; distancia < N_FILES; distancia++) {
            Posicio desti = pos.getVei(deltaFila * distancia, deltaCol * distancia);

            if (!esPosicioValida(desti)) break;
            if (!esCasellaLliure(desti)) break;

            Moviment mov(pos);
            mov.afegeixPosicio(desti);
            moviments.push_back(mov);
        }
    }
}

// ========== CAPTURES ==========

void Tauler::cercaCaptures(const Posicio & pos, ColorFitxa color, TipusFitxa tipus, std::vector<Moviment>&moviments) {
    moviments.clear();

    // Iniciem la cerca recursiva
    Moviment movimentInicial(pos);
    std::vector<Posicio> fitxesJaCapturades;
    cercaCapturesRecursiu(movimentInicial, moviments, fitxesJaCapturades, color, tipus);
}

void Tauler::cercaCapturesRecursiu(Moviment & movimentActual, std::vector<Moviment>&moviments,
    std::vector<Posicio>&fitxesJaCapturades, ColorFitxa color, TipusFitxa tipus) {
    Posicio posActual = movimentActual.getPosicioActual();
    bool capturaFeta = false;

    // Si ja hem fet almenys una captura, afegim aquest moviment com a opció
    // (permet captures parcials)
    if (movimentActual.getNumCaptures() > 0) {
        moviments.push_back(movimentActual);
    }

    int direccions[4][2] = { {-1, -1}, {-1, 1}, {1, -1}, {1, 1} };

    for (int i = 0; i < 4; i++) {
        int deltaFila = direccions[i][0];
        int deltaCol = direccions[i][1];

        if (tipus == FITXA_NORMAL) {
            // IMPORTANT: Les fitxes normals SEMPRE han de mantenir la direcció
            // Blanques sempre cap amunt (deltaFila > 0)
            // Negres sempre cap avall (deltaFila < 0)
            if (!esDireccioValidaPerNormal(deltaFila, color)) {
                continue;  // Saltem aquesta direcció si no és vàlida
            }

            // Fitxa normal: busquem enemic adjacent i posició buida després
            Posicio posEnemic = posActual.getVei(deltaFila, deltaCol);
            Posicio posDesti = posActual.getVei(deltaFila * 2, deltaCol * 2);

            if (potCapturar(posEnemic, posDesti, color, fitxesJaCapturades)) {
                capturaFeta = true;

                // Creem una còpia del moviment per aquesta branca
                Moviment nouMoviment = movimentActual;
                nouMoviment.afegeixPosicio(posDesti);
                nouMoviment.afegeixCaptura(posEnemic);

                // Marquem la fitxa com capturada
                fitxesJaCapturades.push_back(posEnemic);

                // Busquem més captures des de la nova posició
                cercaCapturesRecursiu(nouMoviment, moviments, fitxesJaCapturades, color, tipus);

                // Desmarquem per explorar altres branques
                fitxesJaCapturades.pop_back();
            }
        }
        else { // FITXA_DAMA
            // Les dames SÍ poden moure en totes direccions
            // busquem enemic a qualsevol distància
            for (int dist = 1; dist < N_FILES; dist++) {
                Posicio posCheck = posActual.getVei(deltaFila * dist, deltaCol * dist);

                if (!esPosicioValida(posCheck)) break;

                const Fitxa& fitxa = getFitxa(posCheck);

                if (fitxa.esBuida()) continue; // Seguim buscant

                // Hem trobat una fitxa
                if (fitxa.getColor() != color && !estaCapturada(posCheck, fitxesJaCapturades)) {
                    // És enemic i no està capturat, busquem posició buida després
                    for (int distDesti = dist + 1; distDesti < N_FILES; distDesti++) {
                        Posicio posDesti = posActual.getVei(deltaFila * distDesti, deltaCol * distDesti);

                        if (!esPosicioValida(posDesti)) break;
                        if (!esCasellaLliure(posDesti)) break;

                        // Podem capturar!
                        capturaFeta = true;

                        Moviment nouMoviment = movimentActual;
                        nouMoviment.afegeixPosicio(posDesti);
                        nouMoviment.afegeixCaptura(posCheck);

                        fitxesJaCapturades.push_back(posCheck);
                        cercaCapturesRecursiu(nouMoviment, moviments, fitxesJaCapturades, color, tipus);
                        fitxesJaCapturades.pop_back();
                    }
                }
                break; // No podem saltar més d'una fitxa
            }
        }
    }
}

bool Tauler::potCapturar(const Posicio & posEnemic, const Posicio & posDesti, ColorFitxa color,
    const std::vector<Posicio>&fitxesJaCapturades) const {
    if (!esPosicioValida(posEnemic) || !esPosicioValida(posDesti)) {
        return false;
    }

    const Fitxa& fitxaEnemic = getFitxa(posEnemic);

    return fitxaEnemic.esFitxaJugador() &&
        fitxaEnemic.getColor() != color &&
        esCasellaLliure(posDesti) &&
        !estaCapturada(posEnemic, fitxesJaCapturades);
}

bool Tauler::estaCapturada(const Posicio & pos, const std::vector<Posicio>&fitxesJaCapturades) const {
    for (const Posicio& capturada : fitxesJaCapturades) {
        if (pos == capturada) return true;
    }
    return false;
}
// Elimina una fitxa del tauler
void Tauler::eliminaFitxa(const Posicio& pos) {
    if (esPosicioValida(pos)) {
        m_caselles[pos.getFila()][pos.getColumna()] = Fitxa();
        m_caselles[pos.getFila()][pos.getColumna()].setUbicacio(pos);
    }
}
// ========== EXECUCIÓ DE MOVIMENTS ==========

bool Tauler::mouFitxa(const Posicio & origen, const Posicio & desti) {
    if (!esMovimentValid(origen, desti)) {
        return false;
    }

    // Obtenim el moviment complet
    const Fitxa& fitxa = getFitxa(origen);
    const std::vector<Moviment>& moviments = fitxa.getMoviments();
    Moviment movimentAExecutar;

    for (const Moviment& mov : moviments) {
        if (mov.getPosicioFinal() == desti) {
            movimentAExecutar = mov;
            break;
        }
    }

    // Eliminem les fitxes capturades
    const std::vector<Posicio>& captures = movimentAExecutar.getCaptures();
    for (const Posicio& captura : captures) {
        m_caselles[captura.getFila()][captura.getColumna()] = Fitxa();
        m_caselles[captura.getFila()][captura.getColumna()].setUbicacio(captura);
    }

    // Movem la fitxa
    Fitxa fitxaMoguda = getFitxa(origen);
    fitxaMoguda.setUbicacio(desti);

    m_caselles[desti.getFila()][desti.getColumna()] = fitxaMoguda;
    m_caselles[origen.getFila()][origen.getColumna()] = Fitxa();
    m_caselles[origen.getFila()][origen.getColumna()].setUbicacio(origen);

    // Comprovar promoció
    promocionaFitxa(desti);

    // Actualitzar moviments
    actualitzaMovimentsValids();

    return true;
}

void Tauler::promocionaFitxa(const Posicio & pos) {
    Fitxa& fitxa = m_caselles[pos.getFila()][pos.getColumna()];

    if (fitxa.getTipus() == FITXA_NORMAL) {
        bool haDePromocionar = false;

        if (fitxa.getColor() == COLOR_BLANC && pos.getFila() == N_FILES - 1) {
            haDePromocionar = true;
        }
        else if (fitxa.getColor() == COLOR_NEGRE && pos.getFila() == 0) {
            haDePromocionar = true;
        }

        if (haDePromocionar) {
            fitxa.promocionarADama();
            std::cout << "Fitxa promocionada a dama!" << std::endl;
        }
    }
}

// ========== REGLA DE BUFAT ==========

bool Tauler::hiHaCapturaPossible(ColorFitxa color) const {
    for (int fila = 0; fila < N_FILES; fila++) {
        for (int col = 0; col < N_COLUMNES; col++) {
            const Fitxa& fitxa = m_caselles[fila][col];
            if (fitxa.esFitxaJugador() && fitxa.getColor() == color) {
                std::vector<Moviment> captures;
                Tauler* taulerNoConst = const_cast<Tauler*>(this);
                taulerNoConst->cercaCaptures(fitxa.getUbicacio(), color, fitxa.getTipus(), captures);
                if (!captures.empty()) {
                    return true;
                }
            }
        }
    }
    return false;
}

Moviment Tauler::obteMillorCaptura(ColorFitxa color) const {
    Moviment millorMoviment;
    int maxCaptures = 0;
    int maxDames = 0;

    // Busquem entre totes les fitxes del color
    for (int fila = 0; fila < N_FILES; fila++) {
        for (int col = 0; col < N_COLUMNES; col++) {
            const Fitxa& fitxa = m_caselles[fila][col];
            if (fitxa.esFitxaJugador() && fitxa.getColor() == color) {
                // Obtenim les captures possibles per aquesta fitxa
                std::vector<Moviment> captures;
                Tauler* taulerNoConst = const_cast<Tauler*>(this);
                taulerNoConst->cercaCaptures(fitxa.getUbicacio(), color, fitxa.getTipus(), captures);

                // Busquem la millor captura d'aquesta fitxa
                for (const Moviment& mov : captures) {
                    int numCaptures = mov.getNumCaptures();
                    if (numCaptures > maxCaptures) {
                        maxCaptures = numCaptures;
                        maxDames = comptaDamesCapturades(mov);
                        millorMoviment = mov;
                    }
                    else if (numCaptures == maxCaptures && numCaptures > 0) {
                        int damesActual = comptaDamesCapturades(mov);
                        if (damesActual > maxDames) {
                            maxDames = damesActual;
                            millorMoviment = mov;
                        }
                    }
                }
            }
        }
    }

    return millorMoviment;
}

void Tauler::aplicaReglaBufat(const Posicio & origenMoviment, const Moviment & movimentRealitzat, ColorFitxa colorActual) {
    // Obtenim la millor captura possible
    Moviment millorCaptura = obteMillorCaptura(colorActual);

    // Si el moviment realitzat no és la millor captura, bufem la fitxa
    if (millorCaptura.esMovimentValid() &&
        millorCaptura.getNumCaptures() > movimentRealitzat.getNumCaptures()) {

        std::cout << "BUFAT! No has fet la captura màxima possible!" << std::endl;
        std::cout << "S'elimina la fitxa de " << origenMoviment << std::endl;

        // Eliminem la fitxa que no va capturar correctament
        m_caselles[origenMoviment.getFila()][origenMoviment.getColumna()] = Fitxa();
        m_caselles[origenMoviment.getFila()][origenMoviment.getColumna()].setUbicacio(origenMoviment);
    }
}

int Tauler::comptaDamesCapturades(const Moviment & moviment) const {
    int dames = 0;
    const std::vector<Posicio>& captures = moviment.getCaptures();

    for (const Posicio& captura : captures) {
        const Fitxa& fitxa = getFitxa(captura);
        if (fitxa.esDama()) {
            dames++;
        }
    }

    return dames;
}

// ========== UTILITATS ==========

bool Tauler::esDireccioValidaPerNormal(int deltaFila, ColorFitxa color) const {
    if (color == COLOR_BLANC) {
        return deltaFila > 0;  // Blanques van cap amunt
    }
    else {
        return deltaFila < 0;  // Negres van cap avall
    }
}