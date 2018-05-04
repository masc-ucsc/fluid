package masc

import Chisel._


/** Implements the same interface as chisel3.util.Queue.
  * Effectively a two-entry Queue.
  *  */
class ElasticReg[T <: Data](gen: T) extends Module {
   val entries = 2
   val io = IO(new QueueIO(gen, entries) {})

   private val shaValid = RegInit(false.B)
   private val outValid = RegInit(false.B)
   private val shadow = Reg(gen)
   private val output = Reg(gen)

   // React only if enqueue "or" dequeue
   // enqueue, dequeue need not occur in the same cycle

   when (io.enq.fire() && outValid) {
      shadow := io.enq.bits
      // output flop not getting empty this cycle
      // shadow available for rescue
      shaValid := !io.deq.ready 
   }

   // enq occurs, deq may occur in same cycle
   when (io.enq.fire() && (!outValid || io.deq.ready)) { 
      output := io.enq.bits
      // for the element entering this elastic-reg 
      // when both shadow and output are empty
      outValid := true.B 
   }

   when (io.deq.fire()) {
      // deq occurs, enq may happen in same cycle
      // incoming or shadow occupied
      outValid := io.enq.valid || shaValid 
      when (shaValid) {
         output := shadow
         shaValid := false.B
      }
   }

   io.enq.ready := !shaValid
   io.deq.valid := outValid
   io.deq.bits := output

   io.count := Cat(shaValid & outValid, shaValid ^ outValid)
}

object ElasticReg
{
   def apply[T <: Data](enq: DecoupledIO[T]): DecoupledIO[T] = {
      val q = Module(new ElasticReg(enq.bits.cloneType))
      q.io.enq <> enq
      q.io.deq
   }
}
