# Changelog

A file for documenting significant changes in a project

## [0.1.0] - 2026-04-25

### Added
- **Core Application Architecture**: Established the skeleton of a multi-threaded client-server application with database functionality.
- **Client Module** (`src/client/`):
  - Main client loop and entry point (`main.cpp`)
  - Server connection management (`client_connection/`)
  - Client session handling (`client_session/`)
  - User interface components (`ui/`)
- **Server Module** (`src/server/`):
  - Main server process (`main.cpp`, `server.cpp`)
  - Thread pool for connection handling (`thread_pool/`)
  - Thread-safe queue implementation (`thread_safe_queue.hpp`)
  - Server session logic (`server_session/`)
- **Data Management**:
  - Database module (`db/`) supporting server-side DB and client-side caching
  - SQL scripts for schema initialization (`assets/sql/`)
  - JSON data serialization (`serializer/`, `json/`)
- **Networking Layer**:
  - TCP socket implementation for network communication (`tcp_socket/`)
- **Command System**:
  - Command factory, handlers, and adapters (`command/`)
- **Chat & Messaging**:
  - Chat system and message structures (`chat/`, `message/`)
- **User Management**:
  - Basic user model (`usr/`)
- **Testing**:
  - Test suites for database, chat, messages, and thread pool (`tests/`)
  - GoogleTest integration via submodule
- **Infrastructure**:
  - CMake build system (`CMakeLists.txt`)
  - CI/CD pipeline configuration (`.gitlab-ci.yml`)
  - Build scripts for debug mode and code coverage generation (`scripts/`)
  - Architecture documentation and class diagrams (`docs/`, `assets/uml/`)
  
### Technical Details
- External dependencies: `googletest`, `json`, `spdlog` (included as git submodules)
- Total lines of code added: **~5179 lines** across **70 files**
