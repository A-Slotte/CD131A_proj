#pragma once
#include "Layer.h"
#include <string>

/**
 * @file PresentationLayer.h
 * @brief L6 - Presentationslagret.
*/

/**
 * @brief L6 - Presentation: Kryptering och teckenkodning.
 * 
 * Verkliga motsvarigheter är t.ex. TLS, JPEG och ASCII. I min implementation Enkrypterar lagret datan från L6 och
 * dekrypterar datan från L5. Presentationslagret omvandlar även data mellan intern representation och 
 * en gemensam överföringsrepesentation, hanterar teckenkodning och kompremering.
 * 
 * HEADER
 * Vilken kryptering som används, "ENC=CAESAR".
 * :shift key
 * Enkrypterad data.
 * 
 */
class PresentationLayer : public Layer {
public:
    /**
     * @brief Skickar datan från L7 till encryptering, Enkapsulerar det krypterade
     * meddelandet i sin header och lämnar över till lager 5.
     * 
     */
    void encapsulate(const PacketInfo& packetInfo, std::string sdu) override;
   
    /**
     * @brief Tar emot datan från L5 och packar ur sin header och data fältet. Datan blir decrypterad
     * och skickas till L7. 
     */
    void decapsulate(const PacketInfo& packetInfo, std::string pdu) override;
private:
    std::string encrypt(const PacketInfo& data);
    std::string decrypt(const PacketInfo& data);
};