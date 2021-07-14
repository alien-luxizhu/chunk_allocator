#ifndef __CHUNK_ALLOCATOR_DA5382FF_4444_4416_99D3_2EA9F0081D78
#define __CHUNK_ALLOCATOR_DA5382FF_4444_4416_99D3_2EA9F0081D78
#pragma once
#include <mutex>
#include <type_traits>

namespace stm {
    //only for map/list/queue
    //每次只分配固定尺寸的内存块
    template<typename _Tp> class allocator
    {
        enum { AllocSize = 0xff80 / sizeof(_Tp) };
        struct chunk { chunk* next; };
        class Pool {
            friend class allocator;
            typedef std::lock_guard<std::mutex> lock_t;
            struct Plex { Plex* next; _Tp data[AllocSize]; };
            chunk* free_thunks = nullptr;
            Plex* plextor = nullptr;
            std::mutex _guard;

            void grow() throw()
            {
                Plex* const px = (Plex*)std::malloc(sizeof(Plex));
                px->next = plextor;
                plextor = px;
                pointer p = px->data;
                for (int i = 0; i < AllocSize; i++)
                {
                    chunk* ptr = (chunk*)p++;
                    ptr->next = free_thunks;
                    free_thunks = ptr;
                }
            }

            Pool() {
                free_thunks = nullptr;
                plextor = nullptr;
            }

            ~Pool()
            {
                Plex* p = plextor;
                while (p) {
                    plextor = p->next;
                    std::free(p);
                    p = plextor;
                }
            }
            
            //回收plist中所有_Tp块
            void free(chunk* plist) throw()
            {
                lock_t gLock(_guard);
                chunk* p = plist;
                while (p) {
                    plist = p->next;
                    p->next = free_thunks;
                    free_thunks = p;
                    p = plist;
                }
            }
            //分配一组_Tp块
            chunk* get(int count = 128) throw()
            {
                lock_t gLock(_guard);
                if (!free_thunks)
                    grow();
                chunk *plist = NULL;
                chunk *p = free_thunks;
                for (int i = 0; i < count && p; i++)
                {
                    free_thunks = p->next;
                    p->next = plist;
                    plist = p;
                    p = free_thunks;
                }
                return plist;
            }
        };

    public:
        typedef size_t     size_type;
        typedef ptrdiff_t  difference_type;
        typedef _Tp*       pointer;
        typedef const _Tp* const_pointer;
        typedef _Tp&       reference;
        typedef const _Tp& const_reference;
        typedef _Tp        value_type;

        template<typename _Tp1> struct rebind
        {
            typedef stm::allocator<_Tp1> other;
        };

        typedef std::true_type propagate_on_container_move_assignment;

        allocator() { free_list = nullptr; }

        allocator(const allocator& __a) {}

        allocator(allocator&& __a) {
            free_list = __a.free_list;
            __a.free_list = nullptr;
        }

        template<typename _Tp1>
        allocator(const allocator<_Tp1>&) {
            free_list = _G.get(5);
        }

        ~allocator() throw() {
            _G.free(free_list);
        }

        // 
        pointer address(reference __x) const {
            return std::__addressof(__x);
        }

        const_pointer  address(const_reference __x) const {
            return std::__addressof(__x);
        }

        pointer allocate(size_type __n, const void* = 0) throw()
        {
            if (__n != 1)
                throw std::bad_alloc();
            chunk *ptr = free_list;
            if (ptr == nullptr)
                ptr = _G.get();
            free_list = ptr->next;
            return (pointer)ptr;
        }

        // __p is not permitted to be a null pointer.
        void deallocate(pointer ptr, size_type) {
            chunk* p = (chunk*)(ptr);
            p->next = free_list;
            free_list = p;
        }

        void construct(pointer __p, const _Tp& __val) {
            ::new((void *)__p) _Tp(__val);
        }

        template<typename _Up>
        void destroy(_Up* __p) { __p->~_Up(); }

        inline bool operator!=(const allocator&) const {
            return false;
        }

        inline bool operator==(const allocator&) const {
            return true;
        }
    private:
        //本地二级缓存
        chunk* free_list = nullptr;
        //全局一级缓存
        static Pool _G;
    };

    template<typename _Tp> 
    typename allocator<_Tp>::Pool allocator<_Tp>::_G;

    template<typename _T>
    using fast_allocator = stm::allocator<_T>;

    template<typename _T1, typename _T2>
    inline bool operator==(const allocator<_T1>&, const allocator<_T2>&) {
        return sizeof(_T1) == sizeof(_T2);
    }

    template<typename _T1, typename _T2>
    inline bool operator!=(const allocator<_T1>&, const allocator<_T2>&) {
        return sizeof(_T1) != sizeof(_T2);
    }

#ifdef _STL_MAP_H
    template<typename _Key, typename _Tp,
        typename _Compare = std::less<_Key>,
        typename _Alloc = stm::allocator<std::pair< const _Key, _Tp> > >
        using map = std::map < _Key, _Tp, _Compare, _Alloc>;
#endif

#ifdef _STL_LIST_H
    template<typename _Tp, typename _Alloc = stm::allocator<_Tp> >
    using list = std::list<_Tp, _Alloc>;
#endif

#ifdef _STL_DEQUE_H
    template<typename _Tp, typename _Alloc = stm::allocator<_Tp> >
    using deque = std::deque<_Tp, _Alloc>;
#endif

#ifdef _STL_QUEUE_H
    template<typename _Tp, typename _Sequence = stm::deque<_Tp> >
    using queue = std::queue<_Tp, _Sequence>;
#endif

#ifdef _STL_STACK_H
    template<typename _Tp, typename _Sequence = stm::deque<_Tp> >
    using stack = std::stack<_Tp, _Sequence>;
#endif

#ifdef _STL_SET_H
    template<typename _Key, typename _Compare = std::less<_Key>,
        typename _Alloc = std::allocator<_Key> >
        using set = std::set<_Key, _Compare, _Alloc>;
#endif

}

#endif
