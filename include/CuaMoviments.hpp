#ifndef CUAMOVIMENTS_HPP
#define CUAMOVIMENTS_HPP

#include <string>
#include "../include/moviment.hpp"

class CuaMoviments
{
private:
    struct NodeMoviment {
        Moviment moviment;
        NodeMoviment* seguent;

        NodeMoviment(const Moviment& mov) : moviment(mov), seguent(nullptr) {}
    };

    NodeMoviment* m_primer;
    NodeMoviment* m_ultim;
    int m_mida;

public:
    // Constructor i destructor
    CuaMoviments();
    ~CuaMoviments();

    // Constructor de còpia i operador d'assignació
    CuaMoviments(const CuaMoviments& altra);
    CuaMoviments& operator=(const CuaMoviments& altra);

    // Operacions principals de la cua
    void afegeixMoviment(const Moviment& moviment);
    Moviment obteSeguentMoviment();
    Moviment veureSeguentMoviment() const;

    // Consultes
    bool esBuida() const;
    int getMida() const;

    // Gestió de fitxers
    bool carregaMovimentsDeFitxer(const std::string& nomFitxer);
    bool guardaMovimentsAFitxer(const std::string& nomFitxer) const;

    // Utilitats
    void netejaCua();
    void mostraMoviments() const;

private:
    void alliberaMemoria();
    void copiaNodes(const CuaMoviments& altra);
    Moviment parsejaLiniaMoviment(const std::string& linia) const;
    std::string movimentALinia(const Moviment& moviment) const;
};

#endif // CUAMOVIMENTS_HPP
