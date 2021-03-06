.BowtieQA <- function(x, ...)
{
    new("BowtieQA", .srlist=x, ...)
}

.qa_Bowtie_lane <-
    function(dirPath, pattern, ..., type="Bowtie", 
			 verbose=FALSE)
{
    if (verbose)
        message("qa 'Bowtie' pattern:", pattern)
    rpt <- readAligned(dirPath, pattern, type, ...)
	doc <- .qa_depthOfCoverage(rpt, pattern)
    ac <- .qa_adapterContamination(rpt, pattern, ...)
    alf <- .qa_alphabetFrequency(sread(rpt), baseOnly=TRUE, collapse=TRUE)
    bqtbl <- .qa_alphabetFrequency(quality(rpt), collapse=TRUE)
    rqs <- .qa_qdensity(quality(rpt))
    freqtbl <- tables(sread(rpt))
    abc <- alphabetByCycle(rpt)
    perCycleBaseCall <- .qa_perCycleBaseCall(abc, pattern)
    perCycleQuality <- .qa_perCycleQuality(abc, quality(rpt), pattern)
    aqtbl <- table(quality(alignQuality(rpt)), useNA="always")
    list(readCounts=data.frame(
           read=NA, filter=NA, aligned=length(rpt),
           row.names=pattern),
         baseCalls=data.frame(
           A=alf[["A"]], C=alf[["C"]], G=alf[["G"]], T=alf[["T"]],
           N=alf[["other"]], row.names=pattern),
         readQualityScore=data.frame(
           quality=rqs$x,
           density=rqs$y,
           lane=pattern,
           type="aligned"),
         baseQuality=data.frame(
           score=names(bqtbl),
           count=as.vector(bqtbl),
           lane=pattern),
         alignQuality=data.frame(
           score=as.numeric(names(aqtbl)),
           count=as.vector(aqtbl),
           lane=pattern, row.names=NULL),
         frequentSequences=data.frame(
           sequence=names(freqtbl$top),
           count=as.integer(freqtbl$top),
           type="aligned",
           lane=pattern, row.names=NULL),
         sequenceDistribution=cbind(
           freqtbl$distribution,
           type="aligned",
           lane=pattern),
         perCycle=list(
           baseCall=perCycleBaseCall,
           quality=perCycleQuality),
         perTile=list(
           readCounts=data.frame(
             count=integer(0), type=character(0),
             tile=integer(0), lane=character(0)),
           medianReadQualityScore=data.frame(
             score=integer(), type=character(), tile=integer(),
             lane=integer())),
		 depthOfCoverage=doc,
		 adapterContamination=ac
         )
}

.qa_Bowtie <-
    function(dirPath, pattern, type="Bowtie", ..., 
			 verbose=FALSE)
{
    fls <- .file_names(dirPath, pattern)
    lst <- bplapply(basename(fls), .qa_Bowtie_lane, dirPath=dirPath,
                    type=type, ..., verbose=verbose)
    lst <-
        list(readCounts=.bind(lst, "readCounts"),
             baseCalls=.bind(lst, "baseCalls"),
             readQualityScore=.bind(lst, "readQualityScore"),
             baseQuality=.bind(lst, "baseQuality"),
             alignQuality=.bind(lst, "alignQuality"),
             frequentSequences=.bind(lst, "frequentSequences"),
             sequenceDistribution=.bind(lst, "sequenceDistribution"),
             perCycle=local({
                 lst <- subListExtract(lst, "perCycle")
                 list(baseCall=.bind(lst, "baseCall"),
                      quality=.bind(lst, "quality"))
             }),
             perTile=local({
                 lst <- subListExtract(lst, "perTile")
                 list(readCounts=.bind(lst, "readCounts"),
                      medianReadQualityScore=.bind(
                        lst, "medianReadQualityScore"))
             }),
             depthOfCoverage=.bind(lst, "depthOfCoverage"),
             adapterContamination=.bind(lst, "adapterContamination")
		)
    .BowtieQA(lst)
}

setMethod(report_html, "BowtieQA",
          function(x, dest, type, ...)
{
    qa <- .qa_sampleKey(x)
    dir.create(dest, recursive=TRUE)
    fls <- c("0000-Header.html", "1000-Overview.html",
             "2000-RunSummary.html", "3000-ReadDistribution.html",
             "4000-CycleSpecific.html", "8000-DepthOfCoverage.html",
             "9000-AdapterContamination.html", "9999-Footer.html")
    sections <- system.file("template", fls, package="ShortRead")
    perCycle <- qa[["perCycle"]]
    values <-
        list(SAMPLE_KEY=hwrite(qa[["keyValue"]], border=0),
             PPN_COUNT=.html_img(
               dest, "readCount", .plotReadCount(qa)),
             PPN_COUNT_TBL=hwrite(
               .ppnCount(qa[["readCounts"]]),
               border=0),
             BASE_CALL_COUNT=.html_img(
               dest, "baseCalls", .plotNucleotideCount(qa)),
             READ_QUALITY_FIGURE=.htmlReadQuality(
               dest, "readQuality", qa, "aligned"),
             READ_OCCURRENCES_FIGURE=.htmlReadOccur(
               dest, "readOccurences", qa, "aligned"),
             FREQUENT_SEQUENCES_READ=.html_NA(),
             FREQUENT_SEQUENCES_FILTERED=.html_NA(),
             FREQUENT_SEQUENCES_ALIGNED=hwrite(
               .freqSequences(qa, "aligned"),
               border=0),
             CYCLE_BASE_CALL_FIGURE=.html_img(
               dest, "perCycleBaseCall",
               .plotCycleBaseCall(perCycle$baseCall)),
             CYCLE_QUALITY_FIGURE=.html_img(
               dest, "perCycleQuality",
               .plotCycleQuality(perCycle$quality)),
             DEPTH_OF_COVERAGE_FIGURE=.html_img(
               dest, "depthOfCoverage",
               .plotDepthOfCoverage(qa[["depthOfCoverage"]])),
             ADAPTER_CONTAMINATION=hwrite(
               .df2a(qa[["adapterContamination"]]),
               border=0)
             )
    .report_html_do(dest, sections, values, ...)
})

setGeneric(".bowtie_mismatches",
    function(object, ...) standardGeneric(".bowtie_mismatches"))

setMethod(.bowtie_mismatches, "AlignedRead", function(object, ...)
{
    adata <- alignData(object)
    if (!"mismatch" %in% varLabels(adata))
        .throw(SRError("UserArgumentMismatch",
                       "'%s' does not contain varLabels '%s'",
                       "AlignedDataFrame", "mismatch"))
    if (any(c("nmismatch", "mismatchScore") %in% varLabels(adata)))
        .throw(SRError("UserArgumentMismatch",
                       "'%s' already contains varLabels '%s'",
                       "AlignedDataFrame",
                       "nmismatch', 'mismatchScore'"))
    mmatch <- adata[["mismatch"]]
    idx <- which(nzchar(mmatch))
    if (any(grepl(":", mmatch, fixed=TRUE))) {
        anuc <- lapply(strsplit(mmatch[idx], "[:,]"), "[",
                       c(TRUE, FALSE))
        cidx <- unlist(lapply(anuc, as.integer)) + 1L
    } else {
        anuc <- lapply(strsplit(mmatch[idx], ",", fixed=TRUE),
                       as.integer)
        cidx <- unlist(anuc) + 1L
    }
    len <- sapply(anuc, length)
    ridx <- rep(idx, len)
    x <- as(narrow(quality(object)[ridx], cidx, cidx), "matrix")
    mmscore <- rep(NA_integer_, nrow(adata))
    mmscore[idx] <- unlist(lapply(split(x, ridx), sum), use.names=FALSE)
    lngth <- integer(nrow(adata))
    lngth[idx] <- len

    txt <- "Number of mismatches"
    adata[["nmismatch", labelDescription=txt]] <- lngth
    txt <- "Summed quality scores at mismatched nucleotides"
    adata[["mismatchScore", labelDescription=txt]] <- mmscore
    initialize(object, alignData=adata)
})
