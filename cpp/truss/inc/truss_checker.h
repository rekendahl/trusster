/*
   Trusster Open Source License version 1.0a (TRUST)
   copyright (c) 2006 Mike Mintz and Robert Ekendahl.  All rights reserved.

   Redistribution and use in source and binary forms, with or without modification,
   are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
 this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
 * Redistributions in any form must be accompanied by information on how to obtain
 complete source code for this software and any accompanying software that uses this software.
 The source code must either be included in the distribution or be available in a timely fashion for no more than
 the cost of distribution plus a nominal fee, and must be freely redistributable under reasonable and no more
 restrictive conditions. For an executable file, complete source code means the source code for all modules it
 contains. It does not include source code for modules or files that typically accompany the major components
 of the operating system on which the executable file runs.


 THIS SOFTWARE IS PROVIDED BY MIKE MINTZ AND ROBERT EKENDAHL ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
 INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
 OR NON-INFRINGEMENT, ARE DISCLAIMED. IN NO EVENT SHALL MIKE MINTZ AND ROBERT EKENDAHL OR ITS CONTRIBUTORS
 BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef __truss_checker__
#define __truss_checker__

namespace truss {
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class checker {
        public:
            checker () : expected_check_count (expected_check_count_), actual_check_count (actual_check_count_),
            expected_check_ ("checker"),actual_check_ ("checker") {}
            virtual ~checker() {};

            void wait_expected_check () {expected_check_.wait ();}
            void wait_actual_check () {actual_check_.wait ();}

            virtual void wait_for_completion () = 0;
            virtual void report (const std::string prefix) const = 0;

            const teal::uint32& expected_check_count;
            const teal::uint32& actual_check_count;

        protected:
            virtual void note_expected_check () {expected_check_.signal (); ++expected_check_count_;}
            virtual void note_actual_check ()  {actual_check_.signal ();  ++actual_check_count_;}

            teal::condition expected_check_;
            teal::uint32 expected_check_count_;

            teal::condition actual_check_;
            teal::uint32 actual_check_count_;

        private:
            checker (const checker&); //no impl
            checker& operator= (const checker&); //no impl
    };
};
#endif
