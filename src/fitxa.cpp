#include "../include/fitxa.hpp"
#include <cctype>

// Constructors
Fitxa::Fitxa() : m_tipus(FITXA_BUIDA), m_color(COLOR_BLANC) {
    // Vector es crea buit automàticament
}

Fitxa::Fitxa(TipusFitxa tipus, ColorFitxa color)
    : m_tipus(tipus), m_color(color) {
}

Fitxa::Fitxa(TipusFitxa tipus, ColorFitxa color, const Posicio& pos)
    : m_tipus(tipus), m_color(color), m_ubicacio(pos) {
}

// Gestió de moviments
void Fitxa::afegeixMoviment(const Moviment& moviment) {
    m_movimentsPossibles.push_back(moviment);
}

void Fitxa::netejaMoviments() {
    m_movimentsPossibles.clear();
}

const std::vector<Moviment>& Fitxa::getMoviments() const {
    return m_movimentsPossibles;
}

bool Fitxa::teMovimentsDisponibles() const {
    return !m_movimentsPossibles.empty();
}

// Operacions especials
void Fitxa::promocionarADama() {
    if (m_tipus == FITXA_NORMAL) {
        m_tipus = FITXA_DAMA;
    }
}

// Representació visual
char Fitxa::getCaracter() const {
    switch (m_tipus) {
    case FITXA_BUIDA:
        return CASELLA_BUIDA;
    case FITXA_NORMAL:
        return (m_color == COLOR_BLANC) ? FITXA_BLANCA_NORMAL : FITXA_NEGRA_NORMAL;
    case FITXA_DAMA:
        return (m_color == COLOR_BLANC) ? FITXA_BLANCA_DAMA : FITXA_NEGRA_DAMA;
    default:
        return CASELLA_BUIDA;
    }
}

std::string Fitxa::toString() const {
    return std::string(1, getCaracter());
}

// Validacions
bool Fitxa::esColorContrari(const Fitxa& altra) const {
    // Només comparem si ambdues són fitxes de jugador
    if (esBuida() || altra.esBuida()) {
        return false;
    }
    return m_color != altra.m_color;
}

bool Fitxa::potMoverse() const {
    return esFitxaJugador() && teMovimentsDisponibles();
}

// Operador d'entrada per llegir fitxes des d'arxiu
std::istream& operator>>(std::istream& input, Fitxa& fitxa) {
    char tipusChar;
    std::string posicioStr;

    if (input >> tipusChar >> posicioStr) {
        // Convertim el caràcter a tipus i color
        TipusFitxa tipus;
        ColorFitxa color = COLOR_BLANC;  // Per defecte

        switch (std::toupper(tipusChar)) {
        case 'O':  // FITXA_BLANCA_NORMAL
            tipus = FITXA_NORMAL;
            color = COLOR_BLANC;
            break;
        case 'X':  // FITXA_NEGRA_NORMAL
            tipus = FITXA_NORMAL;
            color = COLOR_NEGRE;
            break;
        case 'D':  // FITXA_BLANCA_DAMA
            tipus = FITXA_DAMA;
            color = COLOR_BLANC;
            break;
        case 'R':  // FITXA_NEGRA_DAMA
            tipus = FITXA_DAMA;
            color = COLOR_NEGRE;
            break;
        default:
            // Si no reconeixem el caràcter, marquem error
            input.setstate(std::ios::failbit);
            return input;
        }

        // Processem la posició
        Posicio pos(posicioStr);
        if (!pos.esValida()) {
            input.setstate(std::ios::failbit);
            return input;
        }

        // Assignem els valors a la fitxa
        fitxa.setTipus(tipus);
        fitxa.setColor(color);
        fitxa.setUbicacio(pos);
    }

    return input;
}