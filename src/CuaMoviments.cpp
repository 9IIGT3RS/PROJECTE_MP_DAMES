#include "../include/CuaMoviments.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

// Constructor
CuaMoviments::CuaMoviments() : m_primer(nullptr), m_ultim(nullptr), m_mida(0) {
}

// Destructor
CuaMoviments::~CuaMoviments() {
    alliberaMemoria();
}

// Constructor de còpia
CuaMoviments::CuaMoviments(const CuaMoviments& altra) : m_primer(nullptr), m_ultim(nullptr), m_mida(0) {
    copiaNodes(altra);
}

// Operador d'assignació
CuaMoviments& CuaMoviments::operator=(const CuaMoviments& altra) {
    if (this != &altra) {
        alliberaMemoria();
        copiaNodes(altra);
    }
    return *this;
}

// Afegeix un moviment al final de la cua
void CuaMoviments::afegeixMoviment(const Moviment& moviment) {
    NodeMoviment* nouNode = new NodeMoviment(moviment);

    if (esBuida()) {
        m_primer = nouNode;
        m_ultim = nouNode;
    }
    else {
        m_ultim->seguent = nouNode;
        m_ultim = nouNode;
    }

    m_mida++;
}

// Obté i elimina el primer moviment de la cua
Moviment CuaMoviments::obteSeguentMoviment() {
    if (esBuida()) {
        std::cout << "Error: Intent d'obtenir moviment d'una cua buida" << std::endl;
        return Moviment();
    }

    Moviment moviment = m_primer->moviment;

    NodeMoviment* nodeAEliminar = m_primer;
    m_primer = m_primer->seguent;

    if (m_primer == nullptr) {
        m_ultim = nullptr;
    }

    delete nodeAEliminar;
    m_mida--;

    return moviment;
}

// Consulta el primer moviment sense eliminar-lo
Moviment CuaMoviments::veureSeguentMoviment() const {
    if (esBuida()) {
        std::cout << "Error: Intent de consultar moviment d'una cua buida" << std::endl;
        return Moviment();
    }

    return m_primer->moviment;
}

// Comprova si la cua està buida
bool CuaMoviments::esBuida() const {
    return m_primer == nullptr;
}

// Obté la mida de la cua
int CuaMoviments::getMida() const {
    return m_mida;
}

// Carrega moviments des d'un fitxer
bool CuaMoviments::carregaMovimentsDeFitxer(const std::string& nomFitxer) {
    netejaCua();

    std::ifstream fitxer(nomFitxer);
    if (!fitxer.is_open()) {
        std::cout << "Error: No s'ha pogut obrir el fitxer: " << nomFitxer << std::endl;
        return false;
    }

    std::string linia;
    int movimentsCarregats = 0;

    while (std::getline(fitxer, linia)) {
        if (!linia.empty()) {
            Moviment moviment = parsejaLiniaMoviment(linia);
            if (moviment.esMovimentValid()) {
                afegeixMoviment(moviment);
                movimentsCarregats++;
            }
        }
    }

    fitxer.close();
    std::cout << "Carregats " << movimentsCarregats << " moviments des de " << nomFitxer << std::endl;
    return movimentsCarregats > 0;
}

// Guarda moviments a un fitxer
bool CuaMoviments::guardaMovimentsAFitxer(const std::string& nomFitxer) const {
    std::ofstream fitxer(nomFitxer);
    if (!fitxer.is_open()) {
        std::cout << "Error: No s'ha pogut crear el fitxer: " << nomFitxer << std::endl;
        return false;
    }

    NodeMoviment* nodeActual = m_primer;
    int movimentsGuardats = 0;

    while (nodeActual != nullptr) {
        std::string linia = movimentALinia(nodeActual->moviment);
        if (!linia.empty()) {
            fitxer << linia << std::endl;
            movimentsGuardats++;
        }
        nodeActual = nodeActual->seguent;
    }

    fitxer.close();
    std::cout << "Guardats " << movimentsGuardats << " moviments a " << nomFitxer << std::endl;
    return true;
}

// Neteja completament la cua
void CuaMoviments::netejaCua() {
    alliberaMemoria();
    m_primer = nullptr;
    m_ultim = nullptr;
    m_mida = 0;
}

// Mostra tots els moviments
void CuaMoviments::mostraMoviments() const {
    std::cout << "=== Contingut de la cua de moviments ===" << std::endl;
    std::cout << "Mida: " << m_mida << std::endl;

    NodeMoviment* nodeActual = m_primer;
    int comptador = 1;

    while (nodeActual != nullptr) {
        std::cout << comptador << ". " << nodeActual->moviment.toString() << std::endl;
        nodeActual = nodeActual->seguent;
        comptador++;
    }

    std::cout << "=======================================" << std::endl;
}

// ========== MÈTODES PRIVATS ==========

void CuaMoviments::alliberaMemoria() {
    while (!esBuida()) {
        NodeMoviment* nodeAEliminar = m_primer;
        m_primer = m_primer->seguent;
        delete nodeAEliminar;
    }
    m_mida = 0;
}

void CuaMoviments::copiaNodes(const CuaMoviments& altra) {
    NodeMoviment* nodeOrigen = altra.m_primer;

    while (nodeOrigen != nullptr) {
        afegeixMoviment(nodeOrigen->moviment);
        nodeOrigen = nodeOrigen->seguent;
    }
}

Moviment CuaMoviments::parsejaLiniaMoviment(const std::string& linia) const {
    std::istringstream iss(linia);
    std::string posicioInici, posicioFinal;

    if (iss >> posicioInici >> posicioFinal) {
        Posicio origen(posicioInici);
        Posicio desti(posicioFinal);

        if (origen.esValida() && desti.esValida()) {
            Moviment moviment(origen);
            moviment.afegeixPosicio(desti);
            return moviment;
        }
    }

    return Moviment();
}

std::string CuaMoviments::movimentALinia(const Moviment& moviment) const {
    if (!moviment.esMovimentValid()) {
        return "";
    }

    Posicio origen = moviment.getPosicioInicial();
    Posicio desti = moviment.getPosicioFinal();

    return origen.toString() + " " + desti.toString();
}