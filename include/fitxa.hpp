#ifndef FITXA_HPP
#define FITXA_HPP

#include <vector>
#include <string>
#include <iostream>
#include "../include/posicio.hpp"
#include "../include/moviment.hpp"
#include "../include/constants.hpp"

enum TipusFitxa {
    FITXA_BUIDA,
    FITXA_NORMAL,
    FITXA_DAMA
};

enum ColorFitxa {
    COLOR_BLANC,
    COLOR_NEGRE
};

class Fitxa
{
private:
    TipusFitxa m_tipus;
    ColorFitxa m_color;
    Posicio m_ubicacio;
    std::vector<Moviment> m_movimentsPossibles;

public:
    // Constructors
    Fitxa();
    Fitxa(TipusFitxa tipus, ColorFitxa color);
    Fitxa(TipusFitxa tipus, ColorFitxa color, const Posicio& pos);

    ~Fitxa() = default;

    // Getters
    TipusFitxa getTipus() const { return m_tipus; }
    ColorFitxa getColor() const { return m_color; }
    Posicio getUbicacio() const { return m_ubicacio; }
    int getNumMoviments() const { return static_cast<int>(m_movimentsPossibles.size()); }

    // Setters
    void setTipus(TipusFitxa tipus) { m_tipus = tipus; }
    void setColor(ColorFitxa color) { m_color = color; }
    void setUbicacio(const Posicio& pos) { m_ubicacio = pos; }

    // Gestió de moviments
    void afegeixMoviment(const Moviment& moviment);
    void netejaMoviments();
    const std::vector<Moviment>& getMoviments() const;
    bool teMovimentsDisponibles() const;

    // Operacions especials
    void promocionarADama();
    bool esDama() const { return m_tipus == FITXA_DAMA; }
    bool esBuida() const { return m_tipus == FITXA_BUIDA; }
    bool esFitxaJugador() const { return m_tipus != FITXA_BUIDA; }

    // Representació visual
    char getCaracter() const;
    std::string toString() const;

    // Validacions
    bool esColorContrari(const Fitxa& altra) const;
    bool potMoverse() const;

    // Operador d'entrada per llegir fitxes des d'arxiu
    friend std::istream& operator>>(std::istream& input, Fitxa& fitxa);
};

#endif // FITXA_HPP
