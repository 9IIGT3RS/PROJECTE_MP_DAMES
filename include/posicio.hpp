#ifndef POSICIO_HPP
#define POSICIO_HPP

#include <iostream>
#include <string>
#include <fstream>
#include "../include/constants.hpp"

class Posicio
{
private:
    int m_fila;
    int m_columna;

public:
    Posicio();
    Posicio(int fila, int columna);
    Posicio(const std::string& notacioEscacs);

    int getFila() const { return m_fila; }
    int getColumna() const { return m_columna; }

    bool setFila(int fila);
    bool setColumna(int columna);
    bool setPosicio(int fila, int columna);

    std::string toString() const;
    void fromString(const std::string& notacioEscacs);

    bool esValida() const;
    static bool esPosicioValida(int fila, int columna);

    Posicio getVei(int deltaFila, int deltaColumna) const;
    bool esDiagonalA(const Posicio& altra) const;

    bool operator==(const Posicio& altra) const;
    bool operator!=(const Posicio& altra) const;

    friend std::ostream& operator<<(std::ostream& os, const Posicio& pos);
    friend std::istream& operator>>(std::istream& is, Posicio& pos);
    friend std::ifstream& operator>>(std::ifstream& fitxer, Posicio& pos);
};

#endif // POSICIO_HPP
