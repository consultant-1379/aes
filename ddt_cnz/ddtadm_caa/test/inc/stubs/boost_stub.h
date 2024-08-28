/*
 * ckpt_stub.h
 *
 *  Created on: Jul 9, 2015
 *      Author: xnadnar
 */

#ifndef INC_STUBS_BOOST_STUB_H_
#define INC_STUBS_BOOST_STUB_H_

#include <map>
#include <list>
#include <string>

namespace boost
{
class mutex
   {
   private:
       mutex(mutex const&);
       mutex& operator=(mutex const&);
   public:
       mutex()
       {

       }
       ~mutex()
       {

       }

       void lock()
       {

       }

       void unlock()
       {

       }
       bool try_lock()
       {
             return true;
       }
   };

   template<typename Lockable>
   class lock_guard
   {
   public:
       explicit lock_guard(Lockable& m_) {}
      // lock_guard(Lockable& m) {}

       ~lock_guard() {}
   };


   namespace archive {

   class text_iarchive
   {
   public:
       text_iarchive(std::istream & is_, unsigned int flags = 0)
       {}
       ~text_iarchive(){}
       template<class T>
       text_iarchive & operator>>(T & t){
       	return *this ;
       }
   };

   class text_oarchive
   {
   public:
       text_oarchive(std::ostream & os_, unsigned int flags = 0)
       {}
       ~text_oarchive(){}

       template<class T>
       text_oarchive & operator<<(T & t){
               return *this ;
           }

   };
   } // namespace archive

}

#endif /* INC_STUBS_BOOST_STUB_H_ */
