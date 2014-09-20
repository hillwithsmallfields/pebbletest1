var raw_org_data = [
    "* Computer stuff						   :computer:",
    "** android							    :android:",
    "*** TODO set up mobile-org to use my dropbox		     :android:online:",
    "    - M8",
    "    - Onda",
    "    - Ubislate",
    "    - Fame",
    "*** TODO find auto-reject call handler for android 		    :android:",
    "*** TODO update endomondo (to add pebble support)",
    "*** OPEN find android app for showing friends where I am	     :online:",
    "*** Galaxy Fame							     :galaxy:",
    "**** TODO copy music and language audio files to Galaxy Fame <2014-06-12 Thu>",
    "**** TODO complete install of osmand and maps on Galaxy Fame",
    "** pebble",
    "*** OPEN interface org-mode to evernote or similar (to pebble)",
    "*** TODO put endomondo onto pebble",
    "*** OPEN write programs to put org agenda onto pebble",
    "** emacs							      :emacs:",
    "*** org-mode",
    "**** TODO org-mode to put table header into window header?	      :emacs:",
    "**** TODO org calendar buffer to bury itself after use		   :computer:",
    "**** OPEN sort out org-agenda-files setting in emacs initialization :computer:emacs:",
    "* Making and mending",
    "** Household",
    "*** OPEN make reading stand <2014-06-15 Sun>",
    "**** OPEN add more glue to reading stand <2014-08-25 Mon>",
    "*** TODO re-pot spider plants <2014-06-03 Tue>",
    "*** TODO make edge barriers round chair platform		   :woodwork:",
    "** Transport",
    "*** Marmalade: see separate file",
    "*** Skoda								:car:",
    "**** TODO choose name for Skoda",
    "     She's Lenka.",
    "**** TODO fit Anderson start connector(s)",
    "*** Trike",
    "**** OPEN smaller lighting system				    :@garage:",
    "**** OPEN LED lower headlamp					    :@garage:",
    "*** Blue bike",
    "**** TODO Refurbish blue bike",
    "**** TODO new lighting system for blue bike",
    "*** Golden trike",
    "**** TODO Refurbish golden trike",
    "**** TODO new lighting system for golden trike",
    "* Other",
    "** CURRENT charge and try oscilloscope <2014-07-26 Sat>		      :@home:",
    "** Nostalgia, collecting, and recording",
    "*** OPEN Find and forward Barrington Hall notes			      :@home:",
    "*** TODO Stick news clippings in scrap book			      :@home:",
    "*** OPEN Stick music fragments to pages				      :@home:"
];

function getSizes(stringarray)
{
    var lines = 0;
    var chars = 0;

    for (var line in stringarray) {
	lines++;
	chars += stringarray[line].length + 1
    }
    return {"lines": lines, "chars": chars}
}

function prepare(raw_strings)
{
    var prepared_strings = [];
    var file_keywords = [];
    var file_dates = [];
    var file_tags = [];
    var heading_pattern = /^(\*+)( ([A-Z]+))? ([^:]*?)( (<[-0-9]+ ...>))?(\s*:([:@a-z]+):)?$/
	for (var line_index in raw_strings) {
	    var matches;
	    raw_string = raw_strings[line_index];
	    print(raw_string);
	    if (matches = raw_string.match(heading_pattern)) {
		print("matched");
		var depth = matches[1].length;
		var keyword = matches[3];
		var keyword_index = file_keywords.indexOf(keyword);
		if (keyword_index == -1) {
		    file_keywords.push(keyword);
		    keyword_index = file_keywords.indexOf(keyword);
		}
		var body = matches[4];
		var date;
		if (matches[6]) {
		    date = matches[6];
		    var date_index = file_dates.indexOf(date);
		    if (date_index == -1) {
			file_dates.push(date);
			date_index = file_dates.indexOf(date);
		    }
		}
		var tags;
		var tag_indices = [];
		if (matches[8]) {
		    tags = matches[8].split(":");
		    for (var i in tags) {
			var tag_index = file_tags.indexOf(tags[i]);
			if (tag_index == -1) {
			    file_tags.push(tags[i]);
			    tag_index = file_tags.indexOf(tags[i]);
			}
			tag_indices.push(tag_index);
		    }
		}
		var constructed_line = depth
		if (keyword) {
		    constructed_line += " !" + keyword_index
		}
		if (tag_indices) {
		    constructed_line += " :" + tag_indices.join(":")
		}
		if (date_index) {
		    constructed_line += " @" + date_index;
		}
		constructed_line += " " + body
		prepared_strings.push(constructed_line)
	    } else {
		prepared_strings.push(raw_string)
	    }
	}

    var dates_line = "@" + file_dates.join(",");
    var tags_line = ":" + file_tags.join(":");
    var keywords_line = "!" + file_keywords.join(" ");

    prepared_strings.unshift(dates_line);
    prepared_strings.unshift(tags_line);
    prepared_strings.unshift(keywords_line);

    sizes = getSizes(prepared_strings);
    prepared_strings.unshift("#/usr/local/bin/zorg " + sizes.lines + " " + sizes.chars);

    return prepared_strings;
}
