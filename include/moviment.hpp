#ifndef MOVIMENT_HPP
#define MOVIMENT_HPP

#include <vector>
#include <string>
#include "../include/posicio.hpp"
#include "../include/constants.hpp"

// Forward declaration para evitar dependencia circular
class Fitxa;

class Moviment
{
private:
    std::vector<Posicio> m_posicions;
    std::vector<Posicio> m_captures;  // Fitxes capturades durant el moviment

public:
    // Constructors
    Moviment();
    Moviment(const Posicio& posicioInicial);
    ~Moviment() = default;

    // Getters
    Posicio getPosicioInicial() const;
    Posicio getPosicioFinal() const;
    Posicio getPosicioActual() const;
    int getNumPosicions() const { return static_cast<int>(m_posicions.size()); }
    int getNumCaptures() const { return static_cast<int>(m_captures.size()); }
    const std::vector<Posicio>& getCaptures() const { return m_captures; }

    // Gestió de posicions
    void afegeixPosicio(const Posicio& pos);
    bool getPosicions(int& numPosicions, Posicio posicions[]) const;

    // Gestió de captures
    void afegeixCaptura(const Posicio& pos);

    // Validacions
    bool esMovimentValid() const;
    bool esMovimentSimple() const;
    bool esMovimentAmbCaptura() const { return !m_captures.empty(); }

    // Representació
    std::string toString() const;

    // Operadors
    bool operator==(const Moviment& altre) const;
};

#endif // MOVIMENT_HPP