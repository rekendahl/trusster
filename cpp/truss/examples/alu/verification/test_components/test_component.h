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
#ifndef __test_component__
#define __test_component__

namespace alu {
  typedef class driver;
  typedef class generator;
  typedef class checker;
};


#include "truss.h"

namespace alu {
  class test_component : public truss::test_component {
  public:
    test_component (const std::string& n, generator* g, driver* b, checker* c);

    virtual void time_zero_setup ();
    virtual void out_of_reset (reset);
    virtual void randomize ();
    virtual void write_to_hardware ();

  protected:
    virtual void generate ();
    virtual void wait_for_completion_ ();
    virtual void start_components_ ();

    generator* generator_;
    driver*    driver_;
    checker*   checker_;

    /*rand*/ teal::uint32 number_of_operations_;
  };

};

#endif
