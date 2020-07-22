#include "Objects/Runner.h"

#include "Cmd.h"

class CmdRunner : public Runner {
   public:
    void run(const char *cmd, Print *out) override {
        String cmdStr{cmd};
        addCommand(cmdStr);
    }
};
