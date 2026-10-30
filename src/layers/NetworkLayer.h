#pragma once
#include "Layer.h"
#include <string>
/**
 * @file NetworkLayer.h
 * @brief L3 – Nätverkslagret.
 */

/**
 * @brief L3 – Nätverk: logisk adressering mellan noder.
 *
 * Motsvarar IP i verkligheten. Varje nod har en logisk nodadress (ett heltal,
 * analogt med en IP-adress). Alla noder tar fysiskt emot alla ramar via
 * broadcast, så det är här som paket till andra noder sorteras bort.
 * TTL skyddar mot routing-loopar; Vidarebefordran är inte implementerad.
 *
 * PDU-format: @c SRC=<adress>;DST=<adress>;TTL=<n>;DATA=<L4-PDU>
 */
class NetworkLayer : public Layer{
public:
    /**
     * @brief Lägger till SRC, DST och TTL och skickar PDU:n till L2.
     * @param packetInfo Information om meddelandet; @c dst och @c ttl används.
     * @param sdu        L4-PDU:n från transportlagret.
     */
    void encapsulate(const PacketInfo& packetInfo, std::string sdu) override;

    /**
     * @brief Kontrollerar DST och TTL och skickar L4-PDU:n uppåt.
     *
     * Avsändar- och mottagaradress skickas vidare i PacketInfo (@c src, @c dst),
     * så att de övre lagren vet vem meddelandet kommer från utan att läsa L3-headern.
     *
     * @param packetInfo Information från datalänklagret.
     * @param pdu        L3-PDU:n från L2.
     * @throws PacketDropped om PDU:n är felformaterad, är till en annan nod eller TTL har gått ut.
     */
    void decapsulate(const PacketInfo& packetInfo, std::string pdu) override;

    void updateSrc(std::string newSrc){
        node = newSrc;
    }
protected:
    /**
     * @brief Kontrollerar att paketets TTL inte har gått ut.
     * @param ttl TTL-värdet ur headern.
     * @return false om TTL är 0 eller lägre (paketet ska kastas).
     */
    bool updateTtl(int ttl);

    /**
     * @brief Kontrollerar om paketet är adresserat till denna nod.
     * @param dstIp DST ur headern.
     * @return true om DST är nodens egen adress.
     */
    bool checkDst(const std::string& dstIp);
    
    std::string node; ///< Nodens egen logiska adress.
};