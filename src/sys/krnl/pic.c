//
// pic.c
//
// Programmable Interrupt Controller (PIC i8259)
//
// Copyright (C) 2002 Michael Ringgaard. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 
// 1. Redistributions of source code must retain the above copyright 
//    notice, this list of conditions and the following disclaimer.  
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.  
// 3. Neither the name of Michael Ringgaard nor the names of its contributors
//    may be used to endorse or promote products derived from this software
//    without specific prior written permission. 
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
// OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
// SUCH DAMAGE.
// 

#include <os/krnl.h>

// All IRQs disabled initially except cascade

unsigned int irq_mask = 0xFFFB; 

//
// Initialize the 8259 Programmable Interrupt Controller
//

void init_pic()
{
  _outp(PIC_MSTR_CTRL, PIC_MSTR_ICW1);
  _outp(PIC_SLV_CTRL, PIC_SLV_ICW1);
  _outp(PIC_MSTR_MASK, PIC_MSTR_ICW2);
  _outp(PIC_SLV_MASK, PIC_SLV_ICW2);
  _outp(PIC_MSTR_MASK, PIC_MSTR_ICW3);
  _outp(PIC_SLV_MASK, PIC_SLV_ICW3);
  _outp(PIC_MSTR_MASK, PIC_MSTR_ICW4);
  _outp(PIC_SLV_MASK, PIC_SLV_ICW4);
  _outp(PIC_MSTR_MASK, PIC_MSTR_DISABLE);
  _outp(PIC_SLV_MASK, PIC_SLV_DISABLE);
}

//
// Enable IRQ
//

void enable_irq(unsigned int irq)
{
  irq_mask &= ~(1 << irq);
  if (irq >= 8) irq_mask &= ~(1 << 2);
	
  _outp(PIC_MSTR_MASK, irq_mask & 0xFF);
  _outp(PIC_SLV_MASK, (irq_mask >> 8) & 0xFF);
}

//
// Disable IRQ
//

void disable_irq(unsigned int irq)
{
  irq_mask |= (1 << irq);
  if ((irq_mask & 0xFF00) == 0xFF00) irq_mask |= (1 << 2);
	
  _outp(PIC_MSTR_MASK, irq_mask & 0xFF);
  _outp(PIC_SLV_MASK, (irq_mask >> 8) & 0xFF);
}

//
// Signal end of interrupt to PIC

void eoi(unsigned int irq)
{
  if (irq < 8)
    _outp(PIC_MSTR_CTRL, irq + PIC_EOI_BASE);
  else
  {
    _outp(PIC_SLV_CTRL, (irq - 8) + PIC_EOI_BASE);
    _outp(PIC_MSTR_CTRL, PIC_EOI_CAS);
  }
}
