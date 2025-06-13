#include "../include/posicio.hpp"
#include <algorithm>
#include <cmath>

// Constructors
Posicio::Posicio() : m_fila(-1), m_columna(-1) {}

Posicio::Posicio(int fila, int columna) : m_fila(fila), m_columna(columna) {}

Posicio::Posicio(const std::string& notacioEscacs) {
    fromString(notacioEscacs);
}

// Setters amb validació
bool Posicio::setFila(int fila) {
    if (fila >= 0 && fila < N_FILES) {
        m_fila = fila;
        return true;
    }
    return false;
}

bool Posicio::setColumna(int columna) {
    if (columna >= 0 && columna < N_COLUMNES) {
        m_columna = columna;
        return true;
    }
    return false;
}

bool Posicio::setPosicio(int fila, int columna) {
    if (esPosicioValida(fila, columna)) {
        m_fila = fila;
        m_columna = columna;
        return true;
    }
    return false;
}

// Mètodes de conversió
std::string Posicio::toString() const {
    if (!esValida()) {
        return "??";
    }

    std::string resultat = "a1";
    resultat[0] = 'a' + m_columna;
    resultat[1] = '1' + m_fila;
    return resultat;
}

void Posicio::fromString(const std::string& notacioEscacs) {
    if (notacioEscacs.length() >= 2) {
        char columnaChar = std::tolower(notacioEscacs[0]);
        char filaChar = notacioEscacs[1];

        if (columnaChar >= 'a' && columnaChar <= 'h' &&
            filaChar >= '1' && filaChar <= '8') {
            m_columna = columnaChar - 'a';
            m_fila = filaChar - '1';
        }
        else {
            m_fila = -1;
            m_columna = -1;
        }
    }
    else {
        m_fila = -1;
        m_columna = -1;
    }
}

// Validacions
bool Posicio::esValida() const {
    return esPosicioValida(m_fila, m_columna);
}

bool Posicio::esPosicioValida(int fila, int columna) {
    return fila >= 0 && fila < N_FILES && columna >= 0 && columna < N_COLUMNES;
}

// Utilitats per moviments
Posicio Posicio::getVei(int deltaFila, int deltaColumna) const {
    return Posicio(m_fila + deltaFila, m_columna + deltaColumna);
}

bool Posicio::esDiagonalA(const Posicio& altra) const {
    int deltaFila = std::abs(m_fila - altra.m_fila);
    int deltaColumna = std::abs(m_columna - altra.m_columna);
    return deltaFila == deltaColumna && deltaFila > 0;
}

// Operadors
bool Posicio::operator==(const Posicio& altra) const {
    return m_fila == altra.m_fila && m_columna == altra.m_columna;
}

bool Posicio::operator!=(const Posicio& altra) const {
    return !(*this == altra);
}

// Operadors d'entrada i sortida
std::ostream& operator<<(std::ostream& os, const Posicio& pos) {
    os << pos.toString();
    return os;
}

std::istream& operator>>(std::istream& is, Posicio& pos) {
    std::string posStr;
    is >> posStr;
    pos.fromString(posStr);
    return is;
}

std::ifstream& operator>>(std::ifstream& fitxer, Posicio& pos) {
    std::string posStr;
    fitxer >> posStr;
    pos.fromString(posStr);
    return fitxer;
}