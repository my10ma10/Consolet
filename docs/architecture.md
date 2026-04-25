# Диаграмма классов

## Цель

Улучшить понимание структуры системы 

## Основные классы
- **DB (ServerDB + ClientCacheDB)**

- CommandInterface
    - NumberedCI
    - SlashedCI
- CommandFactory

<!-- - Client -->
- ClientSession
    - ClientConnection
- Chat
    - ChatType
    - <<"enumeration>> Type
- User
- Message
- Server
    - ServerSession
- ICommand
    - InfoCommand
    - SendMsgCommand
        - NumberedSendMsgCommand
        - SlashedSendMsgCommand
    - PrintChatsListCommand
    - OpenChatCommand
    - ExitAccountCommand
    - QuitAppCommand
- CommandParser

<!-- - UI -- надстройка над CI? -->

- Serializer

## Схема

```mermaid
classDiagram
    %% ----- Commands Core -----
    namespace Commands Core {
        class ICommand

        class SomeCommands
        
        class NumberedSomeCommands
        class SlashedSomeCommands
    }
    note for SomeCommands "SendMsg, Info, PrintChats, Quit..."

    <<interface>> ICommand
    ICommand <|-- SomeCommands

    %% ----- SomeCommand Impl -----
    SomeCommands <|-- NumberedSomeCommands
    SomeCommands <|-- SlashedSomeCommands

    %% ----- Relations -----
    NumberedSomeCommands o-- NumberedCI
    SlashedSomeCommands o-- SlashedCI

    %% ----- CI -----
    namespace CommandInterfaces {
        class CommandInterface

        class NumberedCI
        class SlashedCI
    }
    <<interface>> CommandInterface

    CommandInterface <|.. NumberedCI
    CommandInterface <|.. SlashedCI

    %% ----- Client -----
    namespace Client {
        class ClientSession
        class ClientConnection
    }
    
    ClientConnection --* ClientSession
    CommandInterface --* ClientSession
    ClientCacheDB --o ClientSession
    ClientConnection ..> Serializer

    %% ----- Database -----
    namespace Database {
        class DB
        class ServerDB
        class ClientCacheDB
    }
    DB <|-- ServerDB
    DB <|-- ClientCacheDB


    class Serializer
    
    namespace n_Chat {
        class Chat
        class ChatType
    }

    ChatType --* Chat
    DB --o Chat

    namespace Msg {
        class Message
    }

    Message ..> Serializer
    DB ..> Message


    %% ----- Server -----
    namespace n_Server {
        class Server
        class ServerConnection
    }

    ServerDB --o Server

    Server -- ClientSession : "Обменивается сообщениями"



```


<!-- 
# ...

```mermaid
classDiagram
    classA <|-- classB
    classC *-- classD
    classE o-- classF
    classG <-- classH
    classI -- classJ
    classK <.. classL
    classM <|.. classN
    classO .. classP
``` -->