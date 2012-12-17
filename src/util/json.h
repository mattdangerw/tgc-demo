
/* vim: set et ts=3 sw=3 ft=c:
 *
 * Copyright (C) 2012 James McLaughlin et al.  All rights reserved.
 * https://github.com/udp/json-parser
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _JSON_H
#define _JSON_H

#ifndef json_char
   #define json_char char
#endif

#ifdef __cplusplus

   #include <string>
   #include <cassert>
   #include <glm/glm.hpp>
   using std::string;

   extern "C"
   {

#endif

typedef struct
{
   unsigned long max_memory;
   int settings;

} json_settings;

#define json_relaxed_commas 1

typedef enum
{
   json_none,
   json_object,
   json_array,
   json_integer,
   json_double,
   json_string,
   json_boolean,
   json_null

} json_type;

extern const struct _json_value json_value_none;

typedef struct _json_value
{
   struct _json_value * parent;

   json_type type;

   union
   {
      int boolean;
      long integer;
      double dbl;

      struct
      {
         unsigned int length;
         json_char * ptr; /* null terminated */

      } string;

      struct
      {
         unsigned int length;

         struct
         {
            json_char * name;
            struct _json_value * value;

         } * values;

      } object;

      struct
      {
         unsigned int length;
         struct _json_value ** values;

      } array;

   } u;

   union
   {
      struct _json_value * next_alloc;
      void * object_mem;

   } _reserved;


   /* Some C++ operator sugar */

   #ifdef __cplusplus

      public:

         inline _json_value ()
         {  memset (this, 0, sizeof (_json_value));
         }

         inline const struct _json_value &operator [] (unsigned int index) const
         {
           assert(type == json_array);
           assert(index >= 0 && index < u.array.length);

           return *u.array.values [index];
         }

         inline const struct _json_value &operator [] (const string index) const
         { 
            assert(type == json_object);

            const char *c_index = index.c_str();
            for (unsigned int i = 0; i < u.object.length; ++ i)
               if (!strcmp (u.object.values [i].name, c_index))
                  return *u.object.values [i].value;

            return json_value_none;
         }

         // Adding these to do some type checking and make errors more obvious.
         inline string getNameAt(unsigned int index) const {
            assert(type == json_object);
            return string(u.object.values[index].name);
         }

         inline const struct _json_value &getValueAt(unsigned int index) const {
            assert(type == json_object);
            return *u.object.values[index].value;
         }

         inline int getLength() const {
           assert(type == json_object || type == json_array);
           if (type == json_object) return u.object.length;
           return u.array.length;
         }

         inline float getFloat() const {
           assert(type == json_double);
           return static_cast<float>(u.dbl);
         }

         inline bool getBoolean() const {
           assert(type == json_boolean);
           return u.boolean != 0;
         }

         inline string getString() const {
           assert(type == json_string);
           return string(u.string.ptr);
         }

         inline int getInteger() const {
           assert(type == json_integer);
           return u.integer;
         }

         inline glm::vec2 getVec2() const {
           assert(type == json_array);
           return glm::vec2(u.array.values[0]->getFloat(), u.array.values[1]->getFloat());
         }

         inline glm::vec3 getVec3() const {
           assert(type == json_array);
           return glm::vec3(u.array.values[0]->getFloat(), u.array.values[1]->getFloat(),
                            u.array.values[2]->getFloat());
         }

         inline glm::vec4 getVec4() const {
           assert(type == json_array);
           return glm::vec4(u.array.values[0]->getFloat(), u.array.values[1]->getFloat(),
                            u.array.values[2]->getFloat(), u.array.values[3]->getFloat());
         }
   #endif

} json_value;

json_value * json_parse
   (const json_char * json);

json_value * json_parse_ex
   (json_settings * settings, const json_char * json, char * error);

void json_value_free (json_value *);


#ifdef __cplusplus
   } /* extern "C" */
#endif

#endif


