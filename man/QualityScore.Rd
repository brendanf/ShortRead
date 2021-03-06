\name{QualityScore}
\alias{NumericQuality}
\alias{IntegerQuality}
\alias{MatrixQuality}
\alias{FastqQuality}
\alias{FastqQuality,BStringSet-method}
\alias{FastqQuality,character-method}
\alias{FastqQuality,missing-method}
\alias{SFastqQuality}
\alias{SFastqQuality,BStringSet-method}
\alias{SFastqQuality,character-method}
\alias{SFastqQuality,missing-method}

\title{Construct objects indicating read or alignment quality}
\description{
  Use these functions to construct quality indicators for reads or
  alignments. See \code{\linkS4class{QualityScore}} for details of
  object content and methods available for manipulating them.
}
\usage{
NumericQuality(quality = numeric(0))
IntegerQuality(quality = integer(0))
MatrixQuality(quality = new("matrix"))
FastqQuality(quality, ...)
SFastqQuality(quality, ...)
}

\arguments{

  \item{quality}{An object used to initialize the data
    structure. Appropriate objects are indicated in the constructors
    above for Numeric, Integer, and Matrix qualities. For
    \code{FastqQuality} and \code{SFastqQuality}, methods are defined
    for \code{\link[Biostrings:XStringSet-class]{BStringSet}},
    \code{character}, and \code{missing}.}

  \item{...}{Additional arguments, currently unused.}
}
\value{
  Constructors return objects of the corresponding class derived from
  \code{\linkS4class{QualityScore}}.
}
\author{Martin Morgan <mtmorgan@fhcrc.org>}

\seealso{

  \code{\linkS4class{QualityScore}}, \code{\link{readFastq}},
  \code{\link{readAligned}}

}

\examples{
nq <- NumericQuality(rnorm(20))
nq
quality(nq)
quality(nq[10:1])
}
\keyword{ manip }
