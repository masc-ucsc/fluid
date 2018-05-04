package tests

import org.scalatest._

import chisel3._
import chisel3.util._
import chisel3.iotesters._
import org.scalatest.{Matchers, FlatSpec}

import masc._

import firrtl_interpreter._
import firrtl.{ExecutionOptionsManager, HasFirrtlOptions}



class ElasticRegTests(c: ElasticReg[UInt]) extends PeekPokeTester(c)  {

   val enq = c.io.enq
   val deq = c.io.deq
   val count =  c.io.count

   def enqueueNow(data: BigInt) = {
      while(peek(enq.ready) == BigInt(0)) { step(1) }
      poke(enq.bits.data, data)
      poke(enq.valid, true)
   }
   def resetEnqVal = poke(enq.valid, false)
   def resetDeqReady = poke(deq.ready, false)   
   def expectDequeueNow(data: BigInt) = {
      while(peek(deq.valid) == BigInt(0)) { step(1) }
      expect(deq.bits.data, data)
      poke(deq.ready, true)
   }
   def expectInvalid = expect(deq.valid, false)
   def next = step(1)

   enqueueNow(3)
   expectInvalid
   next
   expectDequeueNow(3)
   enqueueNow(4)
   next
   resetDeqReady
   enqueueNow(1)
   next
   resetEnqVal
   resetDeqReady
   expect(enq.ready,false)
   next
   expectDequeueNow(4)
   expect(enq.ready, false)
   next
   enqueueNow(5)
   expectDequeueNow(1)
   next
   resetDeqReady
   enqueueNow(7)
   expect(count, 1)
   next
   expect(count, 2)
   expectDequeueNow(5)
   expect(enq.ready, false)
}

// Invoke test with:
//    $ sbt 'testOnly boom.unittest.util.ElasticRegTester'
class ElasticRegTester extends FlatSpec with Matchers {
  behavior of "DebugModule"

  val manager1 = new TesterOptionsManager {
    testerOptions = testerOptions.copy(backendName = "firrtl")
    interpreterOptions = interpreterOptions.copy(writeVCD = true)
  }
  val manager2 = new TesterOptionsManager {
    // VCD is generated by default is written in test_run_dir/<test_name>/<module_name>.vcd
    testerOptions = testerOptions.copy(backendName = "verilator")
  }
  it should "check Debug Module " in {
    chisel3.iotesters.Driver.execute(() => new ElasticReg(UInt(3.W)),manager1) ( c => // manager1 or manager2
      new ElasticRegTests(c)
    ) should be(true)
  }
}

