version := "0.1"

name := "masc-fluid"

scalaVersion := "2.11.12"

val defaultVersions = Map(
  "chisel3" -> "latest.release",
  "chisel-iotesters" -> "latest.release"
  )

resolvers ++= Seq(
      "Sonatype Snapshots" at "https://oss.sonatype.org/content/repositories/snapshots",
      "Sonatype Releases" at "https://oss.sonatype.org/content/repositories/releases"
)

libraryDependencies ++= (Seq("chisel3","chisel-iotesters").map {
  dep: String => "edu.berkeley.cs" %% dep % sys.props.getOrElse(dep + "Version", defaultVersions(dep)) })

