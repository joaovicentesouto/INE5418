#include "LocksmithToken.hpp"

namespace locks
{

LocksmithToken::LocksmithToken(const address_type &next_addr) :
    m_next_addr{next_addr}
{
    /*
        Iniciar um thread para ficar esperando para receber o token.
        Se ela receber o token e a thread main não estiver esperando,
        ela passará o token adiante.
    */

}

void LocksmithToken::llock()
{
    /*
        Da lock num semáforo

        Quando a thread UDP receber o token e a thread main
        estiver esperando então é liberada para entrar na
        zona crítica, se não fica esperando.
    */
}

void LocksmithToken::lunlock()
{
    /*
        Sai da zona crítica e libera a thread UDP para mandar o token adiante.
    */
}

} // namespace lock