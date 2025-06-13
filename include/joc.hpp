#ifndef JOC_HPP
#define JOC_HPP

#include <string>
#include <vector>
#include "../include/info_joc.hpp"
#include "../include/tauler.hpp"
#include "../include/CuaMoviments.hpp"
#include "../include/fitxa.hpp"
#include "../include/GraphicManager.h"

class Joc
{
private:
    // Atributs principals
    Tauler m_tauler;
    CuaMoviments m_cua;
    ModeJoc m_modeJoc;

    // Gesti� de fitxers
    std::string m_fitxerTauler;
    std::string m_fitxerMoviments;

    // Estat de la partida
    ColorFitxa m_jugadorActual;
    bool m_partidaAcabada;
    ColorFitxa m_guanyador;

    // Gesti� de selecci� i moviments
    bool m_fitxaSeleccionada;
    Posicio m_posicioSeleccionada;
    std::vector<Posicio> m_posicionsValides;

    // Control d'interacci�
    bool m_mouseAnterior;

    // Per mode replay
    int m_tempsUltimMoviment;

    // Estad�stiques de la partida
    int m_fitxesBlanquesMortes;
    int m_fitxesNegresMortes;

public:
    // Constructor i destructor
    Joc();
    ~Joc() = default;

    // M�todes principals del cicle de vida del joc
    void inicialitza(ModeJoc mode, const std::string& nomFitxerTauler, const std::string& nomFitxerMoviments);
    bool actualitza(int mousePosX, int mousePosY, bool mouseStatus);
    void finalitza();

private:
    // M�todes auxiliars per gesti� del joc
    void inicialitzaModusNormal();
    void inicialitzaModusReplay();
    bool actualitzaModusNormal(int mousePosX, int mousePosY, bool mouseStatus);
    bool actualitzaModusReplay(int mousePosX, int mousePosY, bool mouseStatus);

    // Gesti� d'interacci� amb ratol�
    Posicio converteixCoordenadesMouseATauler(int mousePosX, int mousePosY);
    bool estaDinsLimitsTauler(int mousePosX, int mousePosY);
    void seleccionaFitxa(const Posicio& pos);
    void executaMoviment(const Posicio& desti);
    bool esPosicioValida(const Posicio& pos);

    // Gesti� de torns i final de partida
    void canviaTorn();
    bool comprovaFinalPartida();
    ColorFitxa getGuanyador();

    // M�todes de dibuix
    void dibuixaFons();
    void dibuixaTauler();
    void dibuixaFitxes();
    void dibuixaPosicionsPossibles();
    void dibuixaInformacioPartida();
    void dibuixaPantallaFinal();

    // Utilitats
    bool esFitxaDelJugadorActual(const Posicio& pos);
    IMAGE_NAME obteImatgeFitxa(const Fitxa& fitxa);
    std::string getStringColor(ColorFitxa color);
    std::string getStringModeJoc();
};

#endif // JOC_HPP