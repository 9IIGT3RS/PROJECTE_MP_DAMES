#include "../include/moviment.hpp"
#include <sstream>

// Constructors
Moviment::Moviment() {
}

Moviment::Moviment(const Posicio& posicioInicial) {
    m_posicions.push_back(posicioInicial);
}

// Getters
Posicio Moviment::getPosicioInicial() const {
    if (m_posicions.empty()) {
        return Posicio(-1, -1);
    }
    return m_posicions[0];
}

Posicio Moviment::getPosicioFinal() const {
    if (m_posicions.empty()) {
        return Posicio(-1, -1);
    }
    return m_posicions.back();
}

Posicio Moviment::getPosicioActual() const {
    return getPosicioFinal();
}

// Gestió de posicions
void Moviment::afegeixPosicio(const Posicio& pos) {
    m_posicions.push_back(pos);
}

bool Moviment::getPosicions(int& numPosicions, Posicio posicions[]) const {
    numPosicions = static_cast<int>(m_posicions.size());

    if (numPosicions > MAX_POSICIONS) {
        return false;
    }

    for (int i = 0; i < numPosicions; i++) {
        posicions[i] = m_posicions[i];
    }

    return true;
}

// Gestió de captures
void Moviment::afegeixCaptura(const Posicio& pos) {
    m_captures.push_back(pos);
}

// Validacions
bool Moviment::esMovimentValid() const {
    return m_posicions.size() >= 2;
}

bool Moviment::esMovimentSimple() const {
    return m_posicions.size() == 2 && m_captures.empty();
}

// Representació
std::string Moviment::toString() const {
    if (!esMovimentValid()) {
        return "Moviment no vàlid";
    }

    std::ostringstream oss;

    for (size_t i = 0; i < m_posicions.size(); i++) {
        oss << m_posicions[i].toString();
        if (i < m_posicions.size() - 1) {
            oss << " -> ";
        }
    }

    if (!m_captures.empty()) {
        oss << " (captures: ";
        for (size_t i = 0; i < m_captures.size(); i++) {
            oss << m_captures[i].toString();
            if (i < m_captures.size() - 1) {
                oss << ", ";
            }
        }
        oss << ")";
    }

    return oss.str();
}

// Operadors
bool Moviment::operator==(const Moviment& altre) const {
    if (m_posicions.size() != altre.m_posicions.size() ||
        m_captures.size() != altre.m_captures.size()) {
        return false;
    }

    for (size_t i = 0; i < m_posicions.size(); i++) {
        if (!(m_posicions[i] == altre.m_posicions[i])) {
            return false;
        }
    }

    for (size_t i = 0; i < m_captures.size(); i++) {
        if (!(m_captures[i] == altre.m_captures[i])) {
            return false;
        }
    }

    return true;
}