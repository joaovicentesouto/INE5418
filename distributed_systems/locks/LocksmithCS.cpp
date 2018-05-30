#include "LocksmithCS.hpp"

namespace locks
{

LocksmithCS::LocksmithCS()
{
    std::cout << "Server: ";
    const char* server_address = std::getenv("SERVER_ADDRESS");
    if (server_address)
    {
        std::cout << server_address << '\n';
    }

    std::cout << "Host: ";
    auto hostname = type::ip::host_name();
    std::cout << hostname << '\n';
    /*
        if ( m_server_addr == meu_ip )
            Cria uma thread para ser o servidor.
        else
            Não faz nada, quando a thread main chamar lock então conversa com o servidor.
    */
}

void LocksmithCS::llock()
{
    /*
        UTILIZAR UMA THREAD PARA O CLIENTE:
            ** utiliza mutex
        - Elaborar ...

        SEM THREAD:
            ** utiliza atributo da classe para a conexão
        - Cria-se conexão com o servidor.
        - Utilizar chamada bloqueante com o servidor esperando a
        chance de poder executar.
        - Atendido pelo servidor então podemos ir.
        - Servidor fica esperando pela confirmação da saída da zona crítica.
        - Sai do escopo dessa função.
    */
}

void LocksmithCS::lunlock()
{
    /*
        UTILIZAR UMA THREAD PARA O CLIENTE:
            ** utiliza mutex
        - Elaborar ...

        SEM THREAD:
            ** utiliza atributo da classe para a conexão
        - Avisa o servidor que saiu da zona crítica.
        - Finaliza conexão.
        - Sai do escopo dessa função.
    */
}

} // namespace lock
