#include <piejam/thread/semaphore.h>

#include <semaphore.h>

#include <cassert>
#include <system_error>

namespace piejam::thread
{

struct semaphore::impl
{
    sem_t sem;
};

semaphore::semaphore()
    : semaphore(0)
{
}

semaphore::semaphore(unsigned init_value)
    : m_impl(std::make_unique<impl>())
{
    auto pimpl = std::make_unique<impl>();
    if (-1 == ::sem_init(&pimpl->sem, 0, init_value))
        throw std::system_error(errno, std::generic_category());

    m_impl = std::move(pimpl);
}

semaphore::~semaphore()
{
    if (m_impl.get())
        ::sem_destroy(&m_impl->sem);
}

void
semaphore::acquire()
{
    assert(m_impl);
    ::sem_wait(&m_impl->sem);
}

void
semaphore::release()
{
    assert(m_impl);
    ::sem_post(&m_impl->sem);
}

} // namespace piejam::thread
