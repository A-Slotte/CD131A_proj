#pragma once
#include <iostream>
#include <string>
#include "Layer.h"
#include <map>
#include <vector>
#include <functional>

/**
 * @file ApplicationLayer.h
 * @brief L7 - ApplikationsLagret.
*/

/**
 * @brief L7 - Appliaktion: gränssnittet till användaren.
 * 
 * Motsvarar t.ex HTTP/SMTP i den riktiga OSI-modellen. Tar emot kommandon och text från användaren genom terminalen,
 * skickar text nedåt till presentationslagret (L6) och visar mottagen text.
 * Lagret har ingen egen header utan skickar endast meddelandet.
 * Sparar även konversations historik.
 */
class ApplicationLayer : public Layer {
public:
    /// Datatyp för meddelanden i konversationshistorik.
    struct Message {
        std::string from; /// Avsändare: nodaddress, eller "me" för egna meddelanden.
        std::string text; /// Meddelanden i klartext.
    }; 
    
    /**
     * @brief Skickar meddelande nedåt i stacken.
     * 
     * Sparar meddelandet i historiken och lämnar det till L6. 
     * Paket som kastas påvägen ner fångas här så att programmet fortsätter.
     *  
     * @param packetInfo
     * Addreseringsinformation för meddelandet, t.ex dst, DPORT, cryptkey.
     * @param sdu
     * Meddelandetexten som användaren skrev.
     */
    void encapsulate(const PacketInfo& packetInfo, std::string sdu) override;

    /**
     * @brief Tar emot ett meddelande från L6 och visar det för användaren. Sparar meddelandet i konversationshistoriken.
     * 
     * @param packetInfo 
     * Information från lagren under; @c src används som avsändare. 
     * 
     * @param pdu Avkrypterad meddelande text.
     */
    void decapsulate(const PacketInfo& packetInfo, std::string pdu) override;

    /**
     * @brief Läser kommandon från standard input tills strömmen tar slut.
     *
     * Mottagning sker samtidigt i det fysiska lagrets egen tråd.
     */
    void run();

    /**
     * @brief Tolkar och utför ett kommando från användaren, 
     *
     * Skriver ut felmedddelande om kommandot är okänt eller saknar argument, kör @c help för en lista på kommandon.
     * 
     * @param input Användarens input.
     */
    void handleCommand(const std::string& input);

    /**
     * @brief Delar upp en sträng på blanksteg.
     * 
     * @param str Strängen som ska delas upp.
     * 
     * @return Orden i ordning, utan blanksteg.
     */
    std::vector<std::string> splitWhitespaces(const std::string& str);

     /**
     * @brief Lägger till ett meddelande i historiken.
     * 
     * @param node senders nodadress (nyckeln i historiken).
     * 
     * @param from Vem som skrev meddelandet ("me" eller nodadressen).
     * 
     * @param msg  Meddelandets text.
     */
    void addConversationMessage(const std::string& node, const std::string& from, const std::string& msg);

     /**
     * @brief Skriver ut historiken med en motpart.
     * @param node Motpartens nodadress.
     */
    void printConversation(const std::string& node) const;

    /**
     * @brief Anger hur @c readfile lämnar en bitström till L1.
     *
     * Används bara för att testa avkodningen mot specens testfiler; vanliga
     * meddelanden går alltid genom alla lager.
     *
     * @param injector Funktion som tar emot en bitsträng ("0101...") som om den kom från mediet.
     */
    void setFrameInjector(std::function<void(const std::string&)> injector){
        frameInjector_ = std::move(injector);
    }
    /// Konversationshistorik, key: motpartens nodadress.
    std::map<std::string, std::vector<Message>> conversations;
private:
    void sendMessage(const std::vector<std::string>& command, size_t msgStart, PacketInfo packet);
    void readFrameFile(const std::string& path);
    std::function<void(const std::string&)> frameInjector_;   // går direkt till L1, används bara av readfile
    std::string divider = "========================================";
};