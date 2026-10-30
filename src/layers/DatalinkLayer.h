#pragma once
#include "Layer.h"
#include <string>
#include <cstdint>
#include <iomanip>
#include <sstream>

/**
 * @file DatalinkLayer.h
 * @brief L2 – Datalänklagret.
 */

/**
 * @brief L2 – Datalänk: ramar, länkadresser och felkontroll.
 *
 * Motsvarar Ethernet i verkligheten. Ramen avgränsas med STX/ETX 
 * Ethernets preamble + SFD), MAC_SRC/MAC_DST är länkadresser där FF betyder
 * broadcast, LEN är längden på DATA och CRC är en XOR-checksumma över DATA
 * som upptäcker bitfel från det fysiska lagret.
 *
 * PDU-format: @c STX|MAC_SRC=<id>;MAC_DST=<id>;LEN=<n>;DATA=<L3-PDU>;CRC=<hex>|ETX
 */
class DatalinkLayer : public Layer{
public:
    /**
     * @brief Beräknar LEN och CRC över L3-PDU:n, bygger ramen och skickar den till L1.
     * @param packetInfo Information om meddelandet; @c mac_dst används.
     * @param sdu        L3-PDU:n från nätverkslagret.
     */
    void encapsulate(const PacketInfo& packetInfo, std::string sdu) override;

    /**
     * @brief Kontrollerar ramen och skickar L3-PDU:n uppåt.
     *
     * Kontrollerar i tur och ordning: STX/ETX, MAC_DST (broadcast), att ramen
     * inte är nodens egen, LEN och CRC.
     *
     * @param packetInfo Används inte; L2 skapar ny PacketInfo med MAC-adresserna.
     * @param pdu        Ramen som text från det fysiska lagret.
     * @throws PacketDropped om ramen är felformaterad, är nodens egen eller har bitfel.
     */
    void decapsulate(const PacketInfo& packetInfo, std::string pdu) override;

    void setMac(std::string newMac){
        myMac = newMac;
    }
protected:

    /**
     * @brief XOR av alla bytes i data, som två hexadecimala tecken (specens avsnitt 6.1).
     *
     * Upptäcker alla fel som ändrar ett udda antal bitar, men missar t.ex. två
     * flippar på samma bitposition i två olika bytes (de tar ut varandra).
     *
     * @param sdu Datan som checksumman beräknas över (ramens DATA-fält).
     * @return T.ex. "19".
     */
    std::string xorChecksum(const std::string& sdu);
    std::string myMac;
};