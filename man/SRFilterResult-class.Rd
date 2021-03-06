\name{SRFilterResult-class}
\Rdversion{1.1}
\docType{class}
\alias{SRFilterResult-class}
\alias{SRFilterResult}
\alias{Logic,SRFilterResult,SRFilterResult-method}
\alias{!,SRFilterResult-method}
\alias{name,SRFilterResult-method}
\alias{show,SRFilterResult-method}
\alias{stats}
\alias{stats,SRFilterResult-method}

\title{"SRFilterResult" for SRFilter output and statistics}

\description{

  Objects of this class are logical vectors indicating records passing
  the applied filter, with an associated data frame summarizing the
  name, input number of records, records passing filter, and logical
  operation used for all filters in which the result participated.
  
}

\usage{
SRFilterResult(x = logical(), name = NA_character_,
   input = length(x), passing = sum(x), op = NA_character_)
\S4method{Logic}{SRFilterResult,SRFilterResult}(e1, e2)
\S4method{name}{SRFilterResult}(x, ...)
stats(x, ...)
\S4method{show}{SRFilterResult}(object)
}

\arguments{

  \item{x, object, e1, e2}{For \code{SRFilterResult}, \code{logical()}
    indicating records that passed filter or, for others, an instance of
    \code{SRFilterResult} class.}

  \item{name}{\code{character()} indicating the name by which the filter
    is to be referred. Internally, \code{name}, \code{input},
    \code{passing}, and \code{op} may all be vectors representing
    columns of a \code{data.frame} summarizing the application of
    successive filters.}

  \item{input}{\code{integer()} indicating the length of the original
    input.}

  \item{passing}{\code{integer()} indicating the number of records
    passing the filter.}

  \item{op}{\code{character()} indicating the logical operation, if any,
    associated with this filter.}

  \item{...}{Additional arguments, unused in methods documented on this
    page.}

}

\section{Objects from the Class}{
  
  Objects can be created through \code{\link{SRFilterResult}}, but these
  are automatically created by the application of \code{\link{srFilter}}
  instances.

}

\section{Slots}{
  \describe{

    \item{\code{.Data}:}{Object of class \code{"logical"} indicating
      records that passed the filter. }

    \item{\code{name}:}{Object of class \code{"ScalarCharacter"}
      representing the name of the filter whose results are
      summarized. The name is either the actual name of the filter, or a
      combination of filter names and logical operations when the
      outcome results from application of several filters in a single
      logical expression. }

    \item{\code{stats}:}{Object of class \code{"data.frame"} summarizing
      the name, input number of records, records passing filter, and
      logical operation used for all filters in which the result
      participated. The \code{data.frame} rows correspond either to
      single filters, or to logical combinations of filters.}

  }
}

\section{Extends}{
Class \code{"\linkS4class{logical}"}, from data part.
Class \code{"\linkS4class{.SRUtil}"}, directly.
Class \code{"\linkS4class{vector}"}, by class "logical", distance 2.
Class \code{"\link{atomic}"}, by class "logical", distance 2.
Class \code{"vectorORfactor"}, by class "logical", distance 3.
}
\section{Methods}{
  \describe{

    \item{Logic}{\code{signature(e1 = "SRFilterResult", e2 =
	"SRFilterResult")}: logic operations on filters.}

    \item{!}{\code{signature(x = "SRFilterResult")}: Negate the outcome
      of the current filter results }

    \item{name}{\code{signature(x = "SRFilterResult")}: The name of the
      filter that the results are based on.}

    \item{stats}{\code{signature(x = "SRFilterResult")}: a
      \code{data.frame} as described in the \sQuote{Slots} section of
      this page.}

    \item{show}{\code{signature(object = "SRFilterResult")}: summary of
      filter results.}

  }

}

\author{Martin Morgan \url{mailto:mtmorgan@fhcrc.org}}

\seealso{\code{\link{srFilter}}}

\examples{

fa <- srFilter(function(x) x \%\% 2 == 0, "Even")
fb <- srFilter(function(x) x \%\% 2 == 1, "Odd")

x <- 1:10
fa(x) | fb(x)
fa(x) & fb(x)
!(fa(x) & fb(x))
}

\keyword{classes}
