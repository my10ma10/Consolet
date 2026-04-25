#include "nlohmann/adl_serializer.hpp"
#include "nlohmann/json.hpp"

#include "message/message.hpp"

using json = nlohmann::json;

namespace nlohmann {
    template <>
    struct adl_serializer<Message> {
        static void to_json(json& j, const Message& msg) {
            j = json {
                {"chat_id", msg.chatID_},
                {"sender_id", msg.senderID_},
                {"text", msg.text_},
                {"msg_id", msg.msgID_},
                {"timestamp", msg.timestamp_}
            };
            
        }
        
        static void from_json(const json& j, Message& msg) {
            msg.chatID_ = j.at("chat_id").get<ID_t>();
            msg.senderID_ = j.at("sender_id").get<ID_t>();
            msg.text_ = j.at("text").get<std::string>();
            msg.msgID_ = j.value("msg_id", std::optional<ID_t>{});
            msg.timestamp_ = j.at("timestamp").get<int64_t>();
        }

    };
}
