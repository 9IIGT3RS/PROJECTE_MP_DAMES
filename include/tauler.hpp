#ifndef TAULER_HPP
#define TAULER_HPP

#include <string>
#include <vector>
#include <fstream>
#include "../include/fitxa.hpp"
#include "../include/moviment.hpp"
#include "../include/posicio.hpp"
#include "../include/constants.hpp"

class Tauler
{
private:
    Fitxa m_caselles[N_FILES][N_COLUMNES];

public:
    // Constructor i destructor
    Tauler();
    ~Tauler() = default;

    // Inicialització
    void inicialitzaTaulerBuit();
    bool carregaPartidaDesArxiu(const std::string& nomArxiu);
    void colocaFitxa(const Fitxa& fitxa);

    // Mètodes principals requerits
    void actualitzaMovimentsValids();
    void getPosicionsPossibles(const Posicio& origen, int& numPosicions, Posicio posicionsPossibles[]);
    bool mouFitxa(const Posicio& origen, const Posicio& desti);
    std::string toString() const;

    // Getters
    const Fitxa& getFitxa(const Posicio& pos) const;
    const Fitxa& getFitxa(int fila, int columna) const;
    bool esCasellaLliure(const Posicio& pos) const;

    // Validacions
    bool esPosicioValida(const Posicio& pos) const;
    bool esMovimentValid(const Posicio& origen, const Posicio& desti) const;

    // Operacions sobre fitxes
    void eliminaFitxa(const Posicio& pos);

    // Regla de bufat i obligació de capturar
    bool hiHaCapturaPossible(ColorFitxa color) const;
    Moviment obteMillorCaptura(ColorFitxa color) const;
    void aplicaReglaBufat(const Posicio& origenMoviment, const Moviment& movimentRealitzat, ColorFitxa colorActual);

private:
    // Mètodes auxiliars per calcular moviments
    void calculaMovimentsFitxa(Fitxa& fitxa);
    void calculaMovimentsNormals(const Posicio& pos, ColorFitxa color, std::vector<Moviment>& moviments);
    void calculaMovimentsDama(const Posicio& pos, ColorFitxa color, std::vector<Moviment>& moviments);

    // Gestió de captures
    void cercaCaptures(const Posicio& pos, ColorFitxa color, TipusFitxa tipus, std::vector<Moviment>& moviments);
    void cercaCapturesRecursiu(Moviment& movimentActual, std::vector<Moviment>& moviments,
        std::vector<Posicio>& fitxesJaCapturades, ColorFitxa color, TipusFitxa tipus);
    bool potCapturar(const Posicio& posEnemic, const Posicio& posDesti, ColorFitxa color,
        const std::vector<Posicio>& fitxesJaCapturades) const;
    bool estaCapturada(const Posicio& pos, const std::vector<Posicio>& fitxesJaCapturades) const;

    // Moviments simples (sense captures)
    void cercaMovimentsSimples(const Posicio& pos, ColorFitxa color, TipusFitxa tipus, std::vector<Moviment>& moviments);

    // Execució de moviments
    void executaMoviment(const Moviment& moviment);
    void eliminaFitxesCapturades(const std::vector<Posicio>& captures);
    void promocionaFitxa(const Posicio& pos);

    // Utilitats
    bool esDireccioValidaPerNormal(int deltaFila, ColorFitxa color) const;
    int comptaDamesCapturades(const Moviment& moviment) const;
};

#endif // TAULER_HPP