#pragma once
#include <string>
#include "PacketInfo.h"
#include <vector>
#include <sstream>
#include <iomanip>
#include <iostream>

/**
 * @file Layer.h
 * @brief Gemensam basklass för alla sju OSI-lager.
 */

/**
 * @brief Undantag som kastas när ett lager kastar ett paket.
 *
 * Orsaken skrivs ut av Layer::throwPacket innan undantaget
 * kastas. Fångas i PhysicalLayer (mottagning) och ApplicationLayer (sändning),
 * så att ett trasigt paket aldrig avslutar programmet.
 */

class PacketDropped final {};

/**
 * @brief Basklass för ett lager i protokollstacken.
 *
 * Varje lager känner bara till lagret ovanför (up_) och under (down_), precis
 * som i en riktig nätverksstack. Data skickas nedåt med encapsulate() och
 * uppåt med decapsulate(); 
 *
 * Alla PDU:er är text på formen @c FÄLT=värde;FÄLT=värde;DATA=<SDU>, så
 * klassen innehåller också gemensamma hjälpfunktioner för att tolka dem.
 */

class Layer {
public:
    Layer() = default;

    /**
     * @brief Kopplar ihop lagret med sina grannar i stacken.
     * @param up   Lagret ovanför, eller nullptr för L7.
     * @param down Lagret under, eller nullptr för L1.
    */
    void connect(Layer* up, Layer* down);

    /**
     * @brief Delar en sträng vid ett tecken.
     * @param is    Strängen som ska delas.
     * @param delim Tecknet att dela vid, t.ex. ';' eller '='.
     * @return Delarna i ordning.
     */
    std::vector<std::string> split(const std::string& is, char delim);

    /**
     * @brief Hämtar värdet för ett fält i en header.
     *
     * Exempel: getHeader("SRC=2;DST=5;TTL=4;", "DST") ger "5".
     *
     * @param s    Headern
     * @param targ Fältnamnet att leta efter.
     * @return Fältets värde, eller tom sträng om fältet saknas.
    */
    std::string getHeader(const std::string& s, std::string targ);

    /**
     * @brief Delar en PDU i lagrets header och SDU:n till nästa lager.
     *
     * Delar vid första "DATA=". Eftersom varje lager lägger sin header först
     * hör första "DATA=" alltid till det egna lagret, även om fler "DATA=" kommer efter.
     *
     * @param pdu     Hela PDU:n.
     * @param header  Får allt före "DATA="
     * @param payload Får allt efter "DATA="
     * @return false om PDU:n saknar "DATA=".
     */
    bool splitPdu(const std::string& pdu, std::string& header, std::string& payload);

    /**
     * @brief Skriver ut varför ett paket kastas och kastar PacketDropped.
     * @param layer  Lagrets namn, visas i utskriften.
     * @param reason Orsaken, t.ex. "Frame checksum does not match".
     */
    [[noreturn]] void throwPacket(const std::string& layer, const std::string& reason) const;

    /**
     * @brief Visar vad ett lager just nu gör med ett paket.
     * @param layerName Lagrets namn.
     * @param payload   PDU:n som visas.
     */
    void printLayerStep(const std::string& layerName, const std::string& payload) const {
        std::cout << std::left << std::setw(22) << layerName << " | " << payload << '\n';
    }

    /// Virtuell destruktor: lagren ägs via pekare till basklassen (unique_ptr i main).
    virtual ~Layer() = default;

    /**
     * @brief Tar emot en SDU från lagret ovanför, lägger till egen header och skickar ner.
     * @param packetInfo Information som följer med paketet genom stacken.
     * @param sdu        Data från lagret ovanför.
     */
    virtual void encapsulate(const PacketInfo& packetInfo, std::string sdu) = 0;

    /**
     * @brief Tar emot en PDU från lagret under, kontrollerar och tar bort egen header och skickar upp.
     * @param packetInfo Information från lagren under.
     * @param pdu PDU:n från lagret under.
     * @throws PacketDropped om paketet är felaktigt eller inte är till denna nod.
     */
    virtual void decapsulate(const PacketInfo& packetInfo, std::string pdu) = 0;
    
    /**
     * @brief Antal segment (= SEQ-nummer) som senaste sändningen delades upp i.
     *
     * Standard är 1. TransportLayer överskuggar den så att SessionLayer vet
     * hur många SEQ ett fragmenterat meddelande använde.
     */
    virtual size_t segmentsInLastSend() const { return 1; }

protected:
    Layer* up_ = nullptr;
    Layer* down_ = nullptr;
};