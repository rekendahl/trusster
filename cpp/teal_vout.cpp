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

#include "teal.h"


#if defined (teal_printf_io)
#  define print printf
#else
#  if defined (vpi_2_0)
#    define print vpi_printf
#  else
#    define print io_printf
#  endif
#endif

#include <stdio.h> //HACK for FILE*
#include "time.h"
#include <algorithm>

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
teal::vlog* teal::vlog::the_(0);

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//put in unnamed namespace
class local_vlog : public teal::vlog {
public:
  local_vlog () : vlog ()
  {
    //    std::cout << " local_vlog::local_vlog() " << std::endl;
    pthread_mutex_init (&mutex_, 0);
    time_t dummy;
    ::time (&dummy);
    char m[256];
    sprintf (m, "Start Time: %s\n", ctime (&dummy));
    //    std::cout << " local_vlog::local_vlog() before local print" << std::endl;
    teal::vlog::get().local_print (m);
  }
  
protected:
  teal::message_list output_message_ (const teal::message_list& m) {
    pthread_mutex_lock (&mutex_);  
    teal::message_list foo (teal::vlog::output_message_ (m));
    pthread_mutex_unlock (&mutex_);  
    return foo;
  }
  std::string local_print_ (const std::string& val) {
    print (const_cast<char*> (val.c_str ()));
    if ((fatal_message_seen_) && (val.find ("FATAL") != std::string::npos)) {
      teal::finish ();
    }
    fatal_message_seen_ = false;
    return val;
  }
  pthread_mutex_t mutex_;
};

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
bool match_with_id (const teal::message& m, const teal::message_guide& g) {
  //ok is not exclude and general order/replace guide ?
  return ((g.second.first) && (g.first.second == "") &&
	  (m.first == g.first.first));
}

//needed for older gcc (2.96)
bool operator== (const teal::message& m, const teal::message_guide& g) {
  //ok is not exclude and general order/replace guide ?
  return ((g.second.first) && (g.first.second == m.second) &&
	  (m.first == g.first.first));
}

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
bool match_specific_with_id (const teal::message_guide& g, const teal::message& m) {
  //ok is not exclude, id and string match ?
  //	  std::ostringstream o;
  //	  o << "match specific with id on id " << g.first.first << " msg id " << m.first << 
  //	    "  guide match \"" <<  g.first.second << "\"" <<  " message data of \"" << m.second << "\"\n";
  //	    teal::vlog::get().local_print (o.str ());

  return (
	  (g.first.second == m.second) &&  
	  (m.first == g.first.first));
}

#if 1
//needed for older gcc (2.96)
bool operator== (const teal::message_guide& g, const teal::message& m) {
  //ok is not exclude, id and string match ?
  //  	  std::ostringstream o;
  //  	  o << "match specific with id on id " << g.first.first << " msg id " << m.first << 
  //  	    "  guide match \"" <<  g.first.second << "\"" <<  " message data of \"" << m.second << "\"\n";
  //  	    teal::vlog::get().local_print (o.str ());

  return (0 &&
	  (g.first.second == m.second) &&  
	  (m.first == g.first.first));
}
#endif

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
bool match_on_id_only (const teal::message_guide& g, const teal::message& m) {
  //ok is not exclude, id and string match ?
  return ((m.first == g.first.first) && (g.first.second == "")); //exclude specific requests
}


////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
#if 0
bool match_message (const teal::message& m, const teal::message& exclude) {
#if 1
  std::ostringstream o;
  o << "match message id " << exclude.first << " msg id " << m.first << 
    "  message \"" <<  exclude.second << "\"" <<  " message data of \"" << m.second << "\"\n";
  bool returned ( ((exclude.first == 0) || (m.first == exclude.first)) &&
	   ((exclude.second == "") || (m.second.find (exclude.second) != std::string::npos)) );

  o << " returning: " << returned << "\n";
  teal::vlog::get().local_print (o.str ());
#endif
  return ( ((exclude.first == 0) || (m.first == exclude.first)) &&
	   ((exclude.second == "") || (m.second.find (exclude.second) != std::string::npos)) );
}
#endif

struct match_message : public std::binary_function <teal::message, teal::message, bool> {
  bool operator() (const teal::message& m, const teal::message& exclude) {
#if 0
  std::ostringstream o;
  o << "match message id " << exclude.first << " msg id " << m.first << 
    "  message \"" <<  exclude.second << "\"" <<  " message data of \"" << m.second << "\"\n";
  bool returned ( ((exclude.first == 0) || (m.first == exclude.first)) &&
	   ((exclude.second == "") || (m.second.find (exclude.second) != std::string::npos)) );

  o << " returning: " << returned << "\n";
  teal::vlog::get().local_print (o.str ());
#endif
  return ( ((exclude.first == 0) || (m.first == exclude.first)) &&
	   ((exclude.second == "") || (m.second.find (exclude.second) != std::string::npos)) );
  }
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
teal::file_vlog::file_vlog (std::string file_name, bool a) : out_file_ (0), also_to_screen_ (a) {
  if ((file_name != "") && file_name[0]) out_file_ = fopen ((char*)file_name.c_str (), "w");
  if (out_file_) {
    std::ostringstream o; 
    o << "Output is " <<  ((also_to_screen_) ? "copied " : "sent ") <<  "to " << file_name << " .\n";
    teal::vlog::get().local_print (o.str());
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
teal::file_vlog::~file_vlog () {if (out_file_) {FILE* t = out_file_; out_file_ = 0; fclose (t);}}

 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string teal::file_vlog::local_print_ (const std::string& val) {
  if (out_file_) fprintf (out_file_, const_cast<char*> (val.c_str ()));
  return (also_to_screen_) ? val : "";
}


////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
teal::message_list teal::vlog::output_message_ (const teal::message_list& m)
{
  std::string completed_message;
#if 1
  for (teal::message_list_iterator it5 (m.begin()) ; it5 != m.end(); ++it5) {
    completed_message += (*it5).second;
    ++id_count_[(*it5).first];
    if ((*it5).first == fatal) {
      fatal_message_seen_ = true;
    }
  }
#else
  //multi-pass alg. (1) for the guide specified stuff in guide order (and replacement)
  //(2) for the rest in order found (handling specific replacement)
  for (teal::message_guide_list_iterator it (message_guide_list_.begin ());
       it != message_guide_list_.end (); ++it) {
    //may be more than one message data in the list...
    teal::message_list_iterator it2 (m.begin ());
    do {
      it2 = std::search (it2, m.end (), it, it+1, match_with_id);
      if (it2 != m.end()) {
	{
	  std::ostringstream o;
	  //	  o << "on id " << it->first.first << " found generic guide of \"" <<
	  //	    it->first.second << "\"" <<  " message data of \"" << it2->second << "\"\n";
	  //	    local_print (o.str ());
	}
	teal::message_guide_list_iterator it3 (std::search ( message_guide_list_.begin (),
							     message_guide_list_.end (), it2, it2+1, match_specific_with_id));
	if (it3 != message_guide_list_.end()) {  //some specific guide?
	  {
	    std::ostringstream o;
	    //	    o << "on id " << it->first.first << " found specific guide action of \"" <<
	    //	      it3->second.second << "\"" <<  " message data is \"" << it2->second << "\"\n";
	    // local_print (o.str ());
	  }
	  if (it3->second.first != 0) {
	    if (it3->second.second != "") { //string replacement
	      completed_message += it3->second.second;
	    }
	    else {
	      completed_message += (*it2).second;
	    }
	    ++id_count_[it3->second.first];
	  }
	}
	else {
	  if (it->second.second != "") { //string replacement
	    completed_message += it->second.second;
	  }
	  else {
	    completed_message += (*it2).second;
	  }
	  ++id_count_[it->second.first];
	}
      } //there was some non-exclude guide
      if (it2 != m.end ()) it2++; //find the next one of this guide type
    } while (it2 != m.end ());
  }

  for (teal::message_list_iterator it5 (m.begin()) ; it5 != m.end(); ++it5) {
    teal::message_guide_list_iterator it4 (std::search ( message_guide_list_.begin (),
					      message_guide_list_.end (), it5, it5+1, match_on_id_only));
    if (it4 == message_guide_list_.end ()) {
      {
	std::ostringstream o;
	//		o << "on id " << it5->first << " found no guide. print data of \"" << it5->second << "\"\n";
	bool operator()( const match &a, const match &b ) const {
	  return ((a. < b.));
	}		//	local_print (o.str ());
      }
      completed_message += (*it5).second;
      ++id_count_[(*it5).first];
    }
  }
#endif
  teal::vlog::get().local_print (completed_message);
  return m;
}

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
teal::vlog::vlog () : 
  after_me_ (the_),
  fatal_message_seen_ (false)
{
  for (int i(first_id); (i <= last_id); ++i) {
    message_guide_list_.push_back (message_guide (message (i, ""), message (i,"")));
  }
  the_ = this;
  //  std::cout <<  "vlog::vlog() the_ = " << std::hex << (size_t)the_ << std::endl;
}

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
teal::vlog::~vlog () 
{
  if (the_ == this) {
    assert (after_me_); //ow no printing
    the_ = after_me_;
  }
  else { //take me out of the chain
    vlog* ptr = the_;
    while (ptr && (ptr->after_me_ != this)) ++ptr;
    assert (ptr);
    if (ptr) {ptr->after_me_ = after_me_;} //clip me out
  }
}


////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
teal::vlog& teal::vlog::get () 
{
  //  std::cout <<  "vlog::get() the_ = " << std::hex << (size_t)the_ << std::endl;
  if (!the_) {new local_vlog ();}
  //  std::cout <<  "vlog::get() the_ = " << std::hex << (size_t)the_ << std::endl;
  return *the_;
}


////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
class guide_generic_finder {
 public:
  guide_generic_finder (const teal::message & match) :
    match_ (match) {};
  bool operator() (const teal::message_guide & tester) {
    return ((tester.first.first == match_.first) &&
	    (tester.first.second == "") &&
	    (match_.second == "") );  //do we want a generic?
  }
  const teal::message & match_;
};

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
class guide_specific_finder {
 public:
  guide_specific_finder (const teal::message & match) :
    match_ (match) {};
  bool operator() (const teal::message_guide & tester) {
    return ((tester.first.first == match_.first) &&
	     (tester.first.second == match_.second));
  }
  const teal::message & match_;
};

std::ostream&  print_msg (std::ostream& o, const teal::message& m)
{
  o << "id " << m.first << " string \"" << m.second << "\"";
  return o;
}

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
void teal::vlog::replace (const message& match, const message& action)
{
  //if this is a specific, just add it.
  //if this is a generic, change the one that (may) be there
  //NOTE: There may be both a generic one and a specific one
  teal::message_guide_list_iterator generic = std::find_if (message_guide_list_.begin (), 
					 message_guide_list_.end (), 
					 guide_generic_finder (match));
  teal::message_guide_list_iterator specific = std::find_if (message_guide_list_.begin (), 
					 message_guide_list_.end (), 
					 guide_specific_finder (match));
  if (specific != message_guide_list_.end ()) {
    std::ostringstream o;
    //    o << "on specific guide "; print_msg (o, specific->first) ; 
    //    o << " replacing action of "; print_msg (o, specific->second); 
    //    o << " with  "; print_msg (o, action); o  << "\n";
    //    local_print (o.str ());
    specific->second = action;
  }
  else {
    if (generic != message_guide_list_.end ()) {
      std::ostringstream o;
      //o << "on generic guide "; print_msg (o, generic->first) ; 
      //      o << " replacing action of "; print_msg (o, generic->second); 
      //      o << " with  "; print_msg (o, action);
      //      o << "\n";
      //      local_print (o.str ());
      generic->second = action;
    }
    else {
      std::ostringstream o;
      //      o << "adding new guide of (match) ";  print_msg (o, match);
      //      o << " and action "; print_msg (o, action);
      //      o << "\n";
      //      local_print (o.str ());
      message_guide_list_.push_back (message_guide (match, action));
    }
  }
}

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
void teal::vlog::guide_list (const teal::message_guide_list & l)
{
  message_guide_list_ = l;
}

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
teal::message_guide_list teal::vlog::guide_list () const
{
  return (message_guide_list_);
}

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
unsigned int teal::vlog::how_many (int id)
{
  return (after_me_) ? after_me_->how_many (id) : id_count_ [id];
}



////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

// And now for something completely different

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
teal::vout::vout (const std::string& fa) :
  show_debug_level_ (dictionary::find (fa + "_show_debug_level", (uint32)0)),
  current_line_debug_level_ (0),
  debug_level_ (0), //everything but debug messages
  vout_base_ (hex),
  begin_message_flag_ (true),
  functional_area_ (fa)
{
  pthread_mutex_init (&mutex_, 0);
  //  std::cout << "vout created " << fa << std::endl;
  for (int i (teal::vlog::first_id); (i < teal::vlog::last_id); ++i) {
    message_display (i, true);
  }
}

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
teal::vout::vout (const std::string& fa, uint32 initial_show_level) :
  show_debug_level_ (initial_show_level),
  current_line_debug_level_ (0),
  debug_level_ (0), //everything but debug messages
  vout_base_ (hex),
  begin_message_flag_ (true),
  functional_area_ (fa)
{
  pthread_mutex_init (&mutex_, 0);
  //  std::cout << "vout created " << fa << std::endl;
  for (int i (teal::vlog::first_id); (i < teal::vlog::last_id); ++i) {
    message_display (i, true);
  }
}

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
void teal::vout::base (vout_base a_value) {vout_base_ = a_value;}


////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
teal::vout::vout_base teal::vout::base () { return vout_base_;}


////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
class internal_mutex_sentry {
 public:
  internal_mutex_sentry (pthread_mutex_t* m) : mutex_ (m)
  { pthread_mutex_lock (mutex_); }

  ~internal_mutex_sentry ()
  { pthread_mutex_unlock (mutex_); }

private:
  pthread_mutex_t* mutex_;
};

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
teal::vout& teal::vout::operator<< (char val)
{
  start_a_message_check_ ();
  std::ostringstream o;
  (vout_base_ == dec) ? (o << std::dec << val) :
    (o << "0x" << std::hex << val);
  message_data_ += o.str ();
  return *this;
}


////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
teal::vout& teal::vout::operator<< (unsigned int val)
{
  start_a_message_check_ ();
  std::ostringstream o;
  (vout_base_ == dec) ? (o << std::dec << val) :
    (o << "0x" << std::hex << val);
  message_data_ += o.str ();
  return *this;
}


////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
teal::vout& teal::vout::operator<< (int val)
{
  start_a_message_check_ ();
  std::ostringstream o;
  (vout_base_ == dec) ? (o << std::dec << val) :
    (o << "0x" << std::hex << val);
  message_data_ += o.str ();
  return *this;
}


////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
teal::vout& teal::vout::operator<< (long val)
{
  start_a_message_check_ ();
  std::ostringstream o;
  (vout_base_ == dec) ? (o << std::dec << val) :
    (o << "0x" << std::hex << val);
  message_data_ += o.str ();
  return *this;
}

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
teal::vout& teal::vout::operator<< (long long unsigned int val)
{
  start_a_message_check_ ();
  std::ostringstream o;
  (vout_base_ == dec) ? (o << std::dec << val) :
    (o << "0x" << std::hex << val);
  message_data_ += o.str ();
  return *this;
}


////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
teal::vout& teal::vout::operator<< (const std::string& val)
{
  //  std::cout << functional_area_ << " :vout::operator<< (" << val << ")" << std::endl;
  start_a_message_check_ ();
  std::ostringstream o;
  o << ((vout_base_ == dec) ? std::dec : std::hex) << val;
  message_data_ += o.str ();
  return *this;
}

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
teal::vout& teal::vout::operator<< (double val)
{
  start_a_message_check_ ();
  std::ostringstream o;
  (vout_base_ == dec) ? (o << std::dec << val) :
    (o << "0x" << std::hex << val);
  message_data_ += o.str ();
  return *this;
}


bool operator()( const match &a, const match &b ) const {
  return ((a. < b.));
}#if !defined (teal_printf_io)
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
std::string find_timescale ()
{
  //    std::cout << " vout::find_timescale() " << std::endl;
#if defined (vpi_2_0)
  //  static vpiHandle top_mod_iterator = vpi_iterate (vpiModule, NULL);
  //  static vpiHandle top_mod_handle = vpi_scan (top_mod_iterator);
  //  print ("Top module is %s\n", vpi_get_str (vpiName, top_mod_handle));

  //  switch (vpi_get (vpiTimeUnit, top_mod_handle)) {  //BUG IN IVERLOG??????, null doesn;t work either
  //  switch (vpi_get (vpiTimeUnit, NULL)) {  //BUG IN IVERLOG??????, null doesn;t work either
#if SIM==ivl
  switch (vpi_get (vpiTimePrecision,NULL)) { //just wrong, should ne unit
#else
    switch (vpi_get (vpiTimeUnit, NULL)) {
#endif

#else
  switch (tf_igettimeunit (0)) {
#endif
  case   2: return "* 100s";
  case   1: return "* 10s";
  case   0: return "s";
  case  -1: return "* 100ms";
  case  -2: return "* 10ms";
  case  -3: return "ms";
  case  -4: return "* 100us";
  case  -5: return "* 10us";
  case  -6: return "us";
  case  -7: return "* 100ns";
  case  -8: return "* 10ns";
  case  -9: return "ns";
  case -10: return "* 100ps";
  case -11: return "* 10ps";
  case -12: return "ps";
  case -13: return "* 100fs";
  case -14: return "* 10fs";
  case -15: return "fs";
  default : return "unknown timescale";
  }
  return "unknown timescale";
}


#endif

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
void teal::vout::start_a_message_check_ ()
{
  //  std::cout << functional_area_ << " vout::start_a_message_check_ "  << std::endl;
  if (!begin_message_flag_) return;
  start_a_message_ ();
}

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
bool teal::vout::message_display (int id, bool new_value) 
{
  bool r (message_display_[id]);
  message_display_[id] = new_value;
  return r;
}
  
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
void teal::vout::put_message (int id, std::string msg) 
{
  //  std::cout << functional_area_ << " vout may put message  " << id << "\"" << msg << "\"" << std::endl;
    internal_mutex_sentry foo (&mutex_);

  if (message_display_[id]) {
    message_list_.push_back (message (id, msg));
  }
  //  std::cout << functional_area_ << " :vout::put_message end" << std::endl;
}

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
void teal::vout::start_a_message_ ()
{
  std::ostringstream o;
  o << "[" << vtime () << " " << find_timescale () << "]";
  put_message (vlog::time, o.str ());
  put_message (vlog::functional_area, "[" + functional_area_ + "]");
  put_message (vlog::thread_name, "[" + thread_name (pthread_self()) + "]");
  
  begin_message_flag_ = false;
  //    std::cout << functional_area_ << " vout::start_a_message_() end " << message_data_ << std::endl;
}

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
void teal::vout::end_a_line_ ()
{
  //    std::cout << functional_area_ << " vout end line with data " << message_data_ << std::endl;
  //  if (message_data_ != "")  put_message (vlog::message_data, message_data_);
  //  put_message (vlog::endl, "\n");
  message_data_ += "\n";
}



////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
void teal::vout::end_message_ ()
{
  //  std::cout << functional_area_ << " vout end message  "  << std::endl;
  end_a_line_ ();
  //  put_message (vlog::endl, "\n");
  put_message (vlog::message_data, message_data_);
  //should put this in the set_file_and_line directly???
  if (current_line_debug_level_ <= show_debug_level_) vlog::get().output_message (message_list_);
  clear_message_ ();
}

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
void teal::vout::clear_message_ ()
{
  message_list_.clear ();
  begin_message_flag_ = true;
  file_ = "";
  line_ = (uint32)-1;
  message_data_ = "";
  current_line_debug_level_ = debug_level_; //reset to default
}


////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
void teal::vout::set_file_and_line (const std::string & file, uint32 line) 
{
  start_a_message_check_ ();
  put_message (vlog::file, "[FILE: " + file + "]");
  char temp[256]; //big enough for a uint64
  sprintf (temp, "[line: %d]", line);
  put_message (vlog::line, std::string (temp));
}


