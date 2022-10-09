{
    function ReplaceAll(str, substr, newstr) {
        var p = -1; // 字符出现位置
        var s = 0; // 下一次起始位置

        while ((p = str.indexOf(substr, s)) > -1) {
            s = p + newstr.length; // 位置 + 值的长度
            str = str.replace(substr, newstr);
        }
        return str;
    }
    function normalstr(str) {
        str = ReplaceAll(str, ">", "&gt;");
        str = ReplaceAll(str, "<", "&lt;");
        str = ReplaceAll(str, "\"", "&quot;");
        str = ReplaceAll(str, "\'", "&#39;");
        str = ReplaceAll(str, "\n", "\\n");
        str = ReplaceAll(str, "\r", "\\r");
        return str
    }
    if (!Window.JSON) {
        JSON = {
            parse: function (jsonStr) {
                return eval('(' + jsonStr + ')');
            },
            stringify: function (jsonObj) {
                var result = '',
                    curVal;
                if (jsonObj === null) {
                    return String(jsonObj);
                }
                switch (typeof jsonObj) {
                    case 'number':
                    case 'boolean':
                        return String(jsonObj);
                    case 'string':
                        return '"' + normalstr(jsonObj.toString()) + '"';
                    case 'undefined':
                    case 'function':
                        return undefined;
                }

                switch (Object.prototype.toString.call(jsonObj)) {
                    case '[object Array]':
                        result += '[';
                        for (var i = 0, len = jsonObj.length; i < len; i++) {
                            curVal = JSON.stringify(jsonObj[i]);
                            result += (curVal === undefined ? null : curVal) + ",";
                        }
                        if (result !== '[') {
                            result = result.slice(0, -1);
                        }
                        result += ']';
                        return result;
                    case '[object Date]':
                        return '"' + (jsonObj.toJSON ? jsonObj.toJSON() : jsonObj.toString()) + '"';
                    case '[object RegExp]':
                        return "{}";
                    case '[object Object]':
                        result += '{';
                        for (i in jsonObj) {
                            if (jsonObj.hasOwnProperty(i)) {
                                curVal = JSON.stringify(jsonObj[i]);
                                if (curVal !== undefined) {
                                    result += '"' + i + '":' + curVal + ',';
                                }
                            }
                        }
                        if (result !== '{') {
                            result = result.slice(0, -1);
                        }
                        result += '}';
                        return result;

                    case '[object String]':
                        return '"' + normalstr(jsonObj.toString()) + '"';
                    case '[object Number]':
                    case '[object Boolean]':
                        return jsonObj.toString();
                }
            }
        };
    }
    if (!Object.keys) {
        Object.keys = (function () {
            var hasOwnProperty = Object.prototype.hasOwnProperty,
                hasDontEnumBug = !({ toString: null }).propertyIsEnumerable('toString'),
                dontEnums = [
                    'toString',
                    'toLocaleString',
                    'valueOf',
                    'hasOwnProperty',
                    'isPrototypeOf',
                    'propertyIsEnumerable',
                    'constructor'
                ],
                dontEnumsLength = dontEnums.length;

            return function (obj) {
                if (typeof obj !== 'object' && typeof obj !== 'function' || obj === null) throw new TypeError('Object.keys called on non-object');

                var result = [];

                for (var prop in obj) {
                    if (hasOwnProperty.call(obj, prop)) result.push(prop);
                }

                if (hasDontEnumBug) {
                    for (var i = 0; i < dontEnumsLength; i++) {
                        if (hasOwnProperty.call(obj, dontEnums[i])) result.push(dontEnums[i]);
                    }
                }
                return result;
            }
        })()
    };
    // -- Prototypes
    if (!String.matchWith) {
        String.prototype.matchWith = function (str) {
            return this.indexOf(str) >= 0;
        };
    }
    function parse_ae_elements(thisObj) {
        // read setting file before create UI
        var mySetting = readFile(getSettingsPath());

        var tempFolder, subTempFolder;
        var proFileName = "data.json", pathFileName, thumFileName = "thumbnail.png";
        var separator = "//", outputUrl, rootFolder;

        function onSetupEffects() {
            // comp duplicate
            var previousComps = [];
            var jsonObj = {}, curObj = {};
            var write_file;
            var debugLogTxt = {};
            outputUrl = mySetting.export_path + separator;

            var myFolder = findFolderItemByName("final");
            if (!myFolder) {
                alert('Please make sure you have "Final" folder in the project');
                return;
            }
            var c = checkCompName(myFolder);
            if (c != "") {
                alert(c.name + " in Project is not the correct Naming.");
                return;
            }
            var selections = app.project.selection;
            var comps = [];
            if (selections.length > 0) {
                if (selections.length == 1 && selections[0] instanceof FolderItem && selections[0].name.toUpperCase() == "FINAL") {
                    for (var i = 1; i <= selections[0].numItems; i++) {
                        var comp = selections[0].item(i);
                        comps.push(comp);
                    }
                } else {
                    comps = selections;
                }
            } else {
                // no selections,deal with all comps
                for (var i = 1; i <= myFolder.numItems; i++) {
                    var comp = myFolder.item(i);
                    comps.push(comp);
                }
            }
            // for (var i = 0; i < comps.length; i++) {
            //     var al = alerts(comps[i]);
            //     if (!al) {
            //         return
            //     }
            // }

            // ensure renderQueue hasn't repeat comp
            for (i = 1; i <= app.project.renderQueue.numItems; i++) {
                var curComp = app.project.renderQueue.item(i).comp;
                for (var j = 0; j < comps.length; j++) {
                    if (curComp.name == comps[j].name) {
                        comps.splice(j, 1);
                    }
                }
            }

            if (comps.length == 0) {
                alert("Maybe the comps you selected has existed in renderQueue!");
            } else {
                classificationProcessing(comps);
            }
            function classificationProcessing(comps) {
                tempFolder = findFolderItemByName("_Temp");
                subTempFolder = findFolderItemByName("_subTemp");
                if (!tempFolder) {
                    tempFolder = app.project.items.addFolder("_Temp");
                }
                if (!subTempFolder) {
                    subTempFolder = app.project.items.addFolder("_subTemp");
                    subTempFolder.parentFolder = tempFolder;
                }
                // create output folder
                var folderName = 'AEC_outputs';
                if (app.project.file != null) {
                    folderName = app.project.file.name.split(".")[0];
                }
                folderName = folderName.split("%20").join("_");
                pathFileName = folderName + ".json";
                outputUrl += folderName;
                rootFolder = createFolder(outputUrl);
                // create [project_name].json for writing path
                var filepath = outputUrl + separator + pathFileName;
                write_file = createFile(filepath, 0);
                // read [project_name].json
                debugLogTxt = readFile(write_file);

                var dComps = [];
                for (var i = 0; i < comps.length; i++) {
                    // Duplicate the supplied comp structure
                    var comp = comps[i];
                    jsonObj = {};
                    debugLogTxt[comp.name] = {};
                    comp = tcd_duplicateCompStructure(comp, 1);

                    // create comp folder
                    var folderPath = outputUrl + separator + comp.name;
                    createFolder(folderPath);
                    // get comp and layer property and write it to file
                    compPropertyToJson(comp);
                    layerPropertyToJson(comp);
                    var filepath = outputUrl + separator + comp.name + separator + proFileName;
                    var file = createFile(filepath);
                    // write to data.json,'w' means overwrite
                    var propertyJson = JSON.stringify(jsonObj).split("\\").join("//");
                    // convert [####] to \r\n
                    propertyJson = propertyJson.replace(/(\[####\])/g, '\\r\\n');
                    writeTofile(file, propertyJson, 'w');
                    debugLogTxt[comp.name]["json"] = filepath;
                    var type = getCompType(comp.name.toLowerCase());
                    if (type.matchWith("transition")) {
                        var hasMG = false;
                        for (var j = 0; j < 2; j++) {
                            var tempComp;
                            if (j == 0) {
                                tempComp = comp.duplicate();
                                var obj = kindofHandle(comp, j);
                                comp = obj["comp"];
                                hasMG = obj["hasMG"];
                                dComps.push(comp);
                            } else if (j == 1 && hasMG) {
                                var oldName = comp.name;
                                comp = tempComp;
                                comp.name = "MG_" + oldName;
                                comp = kindofHandle(comp, j)["comp"];
                                dComps.push(comp);
                            }
                        }
                    } else {
                        comp = kindofHandle(comp)["comp"];
                        dComps.push(comp);
                    }
                }
                // output comp to png
                saveAppointFrameToPng(dComps);
            }
            function tcd_duplicateCompStructure(comp, index) {
                // -- Duplicate the incoming comp and set its name
                var newCompName = comp.name;
                var compResult = {};
                compResult.source = comp;
                // get comp duplicate
                var comp = comp.duplicate();
                comp.name = newCompName;
                compResult.dest = comp;
                previousComps.push(compResult);
                if (index == 1) {
                    comp.parentFolder = tempFolder;
                } else {
                    comp.parentFolder = subTempFolder;
                }

                // -- Iterate through the comp and check for subcomps
                for (var i = 1; i <= comp.numLayers; i++) {
                    var layer = comp.layer(i);

                    if (layer instanceof AVLayer) {
                        // The source AVItem for this layer,The value is null in a Text layer
                        // Layer is a comp
                        if (layer.source && layer.source instanceof CompItem) {
                            // Check if this comp has already been duplicated
                            check = tcd_checkPreviousComps(layer.source);

                            if (check != null) {
                                // If so, replace the source with the already duplicated comp
                                tcd_replaceSource(layer, check, false);
                            } else {
                                // If not, duplicate it

                                // Update: Store the previousComps as an object
                                // This allows for faster processing later.
                                var compResult = {};
                                compResult.source = layer.source;

                                // Replace the source of the layer, and recursively check in that subcomp for sub-subcomps
                                var newComp = tcd_duplicateCompStructure(layer.source, 2);
                                tcd_replaceSource(layer, newComp, false);

                                compResult.dest = layer.source;
                                previousComps.push(compResult);
                            }
                        }
                    }
                    if (index == 1) {
                        // set BG to guideLayer
                        var type = getCompType(comp.name.toLowerCase());
                        if (!type.matchWith("transition") && layer.name.matchWith("BG")) {
                            layer.guideLayer = true;
                        }
                    }
                }
                return comp;
            }
            function kindofHandle(comp, index) {
                var hasMG = false;
                for (var i = 1; i <= comp.numLayers; i++) {
                    var layer = comp.layer(i);
                    // set Title\Opener\Lowerthird textlayer to guideLayer
                    var type = getCompType(comp.name.toLowerCase());
                    if ( type.matchWith("title") && layer instanceof TextLayer) {
                        layer.guideLayer = true;
                    }
                    if(type.matchWith("split") && (layer instanceof ShapeLayer || layer instanceof TextLayer)){
                        layer.guideLayer = true;
                    }
                    if (type.matchWith("transition")) {
                        if (layer.name.matchWith("FG_") && index == 0) {
                            layer.Effects.addProperty("Fill").property("Color").setValue([255, 0, 0]);
                        } else if (layer.name.matchWith("FG_") && index == 1) {
                            layer.guideLayer = true;
                        } else if (layer.name.matchWith("BG_")) {
                            layer.guideLayer = true;
                        } else if (layer.name.matchWith("MG_") && index == 0) {
                            hasMG = true;
                            layer.Effects.addProperty("Fill").property("Color").setValue([0, 255, 0]);
                        }
                    }
                }
                return { comp: comp, hasMG: hasMG };
            }
            function createFile(path, flag) {
                var file = File(path);
                if (flag == undefined) {
                    file.remove();
                }
                if (!file.exists) {
                    file = new File(path);
                }
                return file;
            }

            function saveMarkerFrameToPng(comp, time) {
                var path = outputUrl + separator + comp.name + separator + thumFileName;
                var res = comp.saveFrameToPng(time, File(path));
                res.onComplete(function () {
                    debugLogTxt[comp.name]["thumbnail"] = path;
                });
            }
            function writeTofile(file, str, wtype) {
                var out; // our output
                // we know already that the file exist
                // but to be sure
                if (file !== '') {
                    //Open the file for writing.
                    out = file.open(wtype, undefined, undefined);
                    file.encoding = "UTF-8";
                    // file.lineFeed = "Unix"; //convert to UNIX lineFeed
                    file.lineFeed = "Windows";
                    // txtFile.lineFeed = "Macintosh";
                }
                // got an output?
                if (out !== false) {
                    // loop the list and write each item to the file
                    file.write(str);
                    // always close files!
                    file.close();
                }
            }

            function checkTemplate(arr, val) {
                for (key in arr) {
                    if (arr[key] == val) {
                        return true;
                    } else {
                        if (key == arr.length - 1) {
                            return false;
                        }
                    }
                }
            }
            function saveAppointFrameToPng(dComps) {
                var rq_item;
                var tpl = "Best Settings";
                var tpl2 = "AEC_PNG";

                // push compItem into render queue
                for (var i = 0; i < dComps.length; i++) {
                    rq_item = app.project.renderQueue.items.add(dComps[i]);
                    var tplArr = rq_item.templates;
                    var res = checkTemplate(tplArr, tpl);
                    if (!res) {
                        rq_item.remove();
                        alert("make sure you have installed " + tpl + " template in Render Settings");
                        return;
                    }
                    rq_item.applyTemplate(tpl);
                    var om1 = rq_item.outputModule(1);
                    // create folder
                    var tCompName = dComps[i].name;
                    if (dComps[i].name.indexOf("MG_") == 0) {
                        tCompName = dComps[i].name.substring(3);
                    }
                    var sequence = outputUrl + separator + tCompName + separator + "sequence";
                    createFolder(sequence);
                    tplArr = om1.templates;
                    res = checkTemplate(tplArr, tpl2);
                    if (!res) {
                        rq_item.remove();
                        alert("make sure you have installed " + tpl2 + " template in OutputModule")
                        return;
                    }
                    om1.applyTemplate(tpl2);
                    var path2 = sequence + separator + dComps[i].name + "_[#####].png";
                    om1.file = File(path2); // set the output path
                    if (dComps[i].name.indexOf("MG_") < 0) {
                        var path = sequence + separator + tCompName + "_%04d.png";
                        debugLogTxt[dComps[i].name]["sequence"] = path;
                        debugLogTxt[dComps[i].name]["start"] = dComps[i].displayStartTime * dComps[i].frameRate;
                        debugLogTxt[dComps[i].name]["end"] = (dComps[i].displayStartTime + dComps[i].duration) * dComps[i].frameRate;
                    }
                    if (i == dComps.length - 1) {
                        fileCorrespondence(debugLogTxt);

                        // write to [project_name].json,'w' means overwrite
                        writeTofile(write_file, JSON.stringify(debugLogTxt), 'w');
                        var debugeLog = getLogPath();
                        var content = dealwithPath(write_file, 0);
                        // write to debuge.log,'a' means append
                        writeTofile(debugeLog, content + '\r\n', 'a');
                    }
                }
            }
            function fileCorrespondence(debugLogTxt) {
                var rootFile = rootFolder.getFiles();
                for (var i = 0; i < rootFile.length; i++) {
                    var curFile = rootFile[i];
                    if (curFile instanceof Folder) {
                        for (var j = 1; j <= app.project.renderQueue.numItems; j++) {
                            var curComp = app.project.renderQueue.item(j).comp;
                            if (curFile.name == curComp.name) {
                                break;
                            } else {
                                if (j == app.project.renderQueue.numItems) {
                                    delete debugLogTxt[curFile.name];
                                }
                            }
                        }
                    }
                }
            }


            function compPropertyToJson(comp) {
                jsonObj[comp.name] = {
                    "width": comp.width,
                    "height": comp.height,
                    "pixelAspect": comp.pixelAspect,
                    "frameRate": comp.frameRate,
                    "frameDuration": comp.frameDuration,
                    "duration": comp.duration,
                    "useProxy": comp.useProxy,
                    "proxySource": comp.proxySource,
                    "time": comp.time,
                    "usedIn": comp.usedIn,
                    "hasVideo": comp.hasVideo,
                    "hasAudio": comp.hasAudio,
                    "footageMissing": comp.footageMissing,
                    "frameDuration": comp.frameDuration,
                    "dropFrame": comp.dropFrame,
                    "workAreaStart": comp.workAreaStart,
                    "workAreaDuration": comp.workAreaDuration,
                    "numLayers": comp.numLayers,
                    "hideShyLayers": comp.hideShyLayers,
                    "motionBlur": comp.motionBlur,
                    "draft3d": comp.draft3d,
                    "frameBlending": comp.frameBlending,
                    "preserveNestedFrameRate": comp.preserveNestedFrameRate,
                    "bgColor": comp.bgColor,
                    "activeCamera": comp.activeCamera,
                    "displayStartTime": comp.displayStartTime,
                    "resolutionFactor": comp.resolutionFactor,
                    "shutterAngle": comp.shutterAngle,
                    "shutterPhase": comp.shutterPhase,
                    "motionBlurSamplesPerFrame": comp.motionBlurSamplesPerFrame,
                    "motionBlurAdaptiveSampleLimit": comp.motionBlurAdaptiveSampleLimit,
                    "selectedLayers": comp.selectedLayers,
                    "selectedProperties": comp.selectedProperties,
                    "renderer": comp.renderer,
                    "renderers": comp.renderers,
                    "Markers": []
                };
                getCompMarkers(comp);
            }
            function layerPropertyToJson(comp) {
                jsonObj[comp.name]["layers"] = [];
                for (var i = 1; i <= comp.numLayers; i++) {
                    var layer = comp.layer(i);
                    curObj = {};
                    // get textlayer property
                    if (layer instanceof TextLayer) {
                        curObj = {
                            "index": layer.index,
                            "name": layer.name,
                            "blendingMode": layer.blendingMode,
                            "Parent&Link": layer.parent ? layer.parent.index : "",
                            "startTime": layer.startTime,
                            "outPoint": layer.outPoint,
                            "inPoint": layer.inPoint
                        };
                        for (var ii = 1; ii <= layer.numProperties; ii++) {
                            var name = layer.property(ii).name;
                            curObj[name] = {};
                            iterateLayerProperty("textLayer", layer[name], curObj[name]);
                        }
                    } else if (layer.source && layer.source instanceof CompItem) {
                        curObj = {
                            "index": layer.index,
                            "name": layer.name,
                            "blendingMode": layer.blendingMode,
                            "Parent&Link": layer.parent ? layer.parent.index : "",
                            "startTime": layer.startTime,
                            "outPoint": layer.outPoint,
                            "inPoint": layer.inPoint
                        };
                        // get compLayer property
                        for (var ii = 1; ii <= layer.numProperties; ii++) {
                            var name = layer.property(ii).name;
                            if (name == "Time Remap") {
                                curObj[name] = layer[name].value;
                            } else {
                                curObj[name] = {};
                                iterateLayerProperty("compLayer", layer[name], curObj[name]);
                            }
                        }
                    } else if(layer instanceof ShapeLayer){
                        curObj = {
                            "index": layer.index,
                            "name": layer.name,
                            "blendingMode": layer.blendingMode,
                            "Parent&Link": layer.parent ? layer.parent.index : "",
                            "startTime": layer.startTime,
                            "outPoint": layer.outPoint,
                            "inPoint": layer.inPoint
                        };
                        for (var ii = 1; ii <= layer.numProperties; ii++) {
                            var name = layer.property(ii).name;
                        
                            curObj[name] = {};
                            iterateLayerProperty("shapeLayer", layer[name], curObj[name]);
                        }
                    }else if(layer.nullLayer){
                        curObj = {
                            "index": layer.index,
                            "name": layer.name,
                            "blendingMode": layer.blendingMode,
                            "Parent&Link": layer.parent ? layer.parent.index : "",
                            "startTime": layer.startTime,
                            "outPoint": layer.outPoint,
                            "inPoint": layer.inPoint
                        };
                        for (var ii = 1; ii <= layer.numProperties; ii++) {
                            var name = layer.property(ii).name;
                            curObj[name] = {};
                            iterateLayerProperty("nullLayer", layer[name], curObj[name]);
                        }
                    }
                    if (Object.keys(curObj).length > 0) {
                        jsonObj[comp.name]["layers"].push(curObj);
                    }
                }
            }
            // iterate layer property
            function iterateLayerProperty(layerType, pro, jObj) {
                if (pro.numProperties && pro.numProperties > 0) {
                    for (var i = 1; i <= pro.numProperties; i++) {
                        var cur = pro(i);
                        if (cur.numProperties && cur.numProperties > 0) {
                            jObj[cur.name] = {};
                            iterateLayerProperty(layerType, cur, jObj[cur.name]);
                            if(layerType == "shapeLayer" && cur.matchName == 'ADBE Vector Shape - Group'){
                                var elem = cur.property('Path').valueAtTime(0,true).vertices;
                                jObj[cur.name]["Vertices"] = elem;
                            }
                        } else {
                            // some value is readOnly or other type,via check propertyValueType 
                            if ((cur.propertyValueType != PropertyValueType.NO_VALUE) && cur.propertyValueType != PropertyValueType.CUSTOM_VALUE) {
                                if (cur.numKeys > 0) {
                                    jObj[cur.name] = {};
                                    jObj[cur.name]["value"] = cur.value;
                                    for (var j = 1; j <= cur.numKeys; j++) {
                                        jObj[cur.name]["keyFrame" + j] = {
                                            "keyValue": cur.keyValue(j),
                                            "keyTime": cur.keyTime(j),
                                            "influence": cur.keyInTemporalEase(j)[0].influence,
                                            "speed": cur.keyInTemporalEase(j)[0].speed,
                                        };
                                    }
                                } else {
                                    if (layerType == "textLayer") {
                                        if (cur.name != "Source Text") {
                                            jObj[cur.name] = cur.value;
                                        } else {
                                            var obj = cur.value;
                                            var keys = Object.keys(obj);
                                            var res = {};
                                            for (var i = 0; i < keys.length; i++) {
                                                try {
                                                    var str = obj[keys[i]];
                                                    if (keys[i] == 'text') {
                                                        str = encodeURIComponent(str);
                                                        // convert 'newline' to [####]
                                                        str = str.replace(/(%0D|%0d)/gm, '[####]');
                                                        str = decodeURIComponent(str);
                                                    }
                                                    res[keys[i]] = str;
                                                } catch (err) {
                                                }
                                            }
                                            jObj[cur.name] = res;
                                        }
                                    }else if (layerType == "compLayer"  || layerType == "nullLayer" ||layerType == "shapeLayer") {
                                        jObj[cur.name] = cur.value;
                                    }
                                }
                            }
                        }
                    }
                } else if (pro.propertyValueType == PropertyValueType.MARKER) {//Marker info
                    var obj = pro.value;
                    var keys = Object.keys(obj);
                    for (var i = 0; i < keys.length; i++) {
                        jObj[keys[i]] = obj[keys[i]];
                    }
                }
            }

            function parseShape(shapeInfo,shapeOb,time){
                var shapeContents = shapeInfo.property('Contents');

                var paths = [];
                var numProperties = shapeContents.numProperties;
                for(var i = 0;i<numProperties;i+=1){
                    if(shapeContents(i+1).matchName == 'ADBE Vector Shape - Group'){
                        paths.push(shapeContents(i+1));
                    }
                }       
                if(paths.length>0){
                    if(shapeOb.an.path){
                        parsePaths(paths,shapeOb.an.path, shapeOb.lastData, time);
                    }
                }   
            }

            function tcd_checkPreviousComps(comp) {
                // Check the list of previous comps for the specified item's ID
                // to make sure it isn't duplicated twice
                for (var i = 0; i < previousComps.length; i++) {
                    if (previousComps[i].source.id == comp.id) { return previousComps[i].dest; }
                }
                return null;
            }
            function tcd_replaceSource(layer, newItem, fixExp) {
                // Replace Source, placeholder for any future improvements needed in this area
                layer.replaceSource(newItem, fixExp);
            }
            function alerts(comp) {
                if (comp instanceof CompItem) {
                    var markerNum = getCompMarkers(comp, 1);
                    if (markerNum != 1) {
                        alert("Ensure you have one Marker in " + comp.name);
                        return false;
                    }
                    var res = checkLayer(comp);
                    var type = getCompType(comp.name.toLowerCase());
                    if (!type.matchWith("transition") && res.count != 1) {
                        alert(comp.name + " have more layers than it needs");
                        return false;
                    } else if (type.matchWith("transition")) {
                        if (res.count > 1) {
                            alert(comp.name + " have more layers than it needs");
                            return false;
                        }
                        if (!res.BG || !res.FG) {
                            alert(comp.name + " doesn't have the FG or BG");
                            return false;
                        }
                    }
                    if (res.Layer) {
                        alert(comp.name + " have wrong layer");
                        return false;
                    }
                }
                return true;
            }

            function checkLayer(comp) {
                var count = 0;
                var BG = false;
                var FG = false;
                var Layer = false;
                for (var i = 1; i <= comp.numLayers; i++) {
                    var layer = comp.layer(i);
                    if (!(layer instanceof TextLayer) && !layer.name.matchWith("BG_") && !layer.name.matchWith("FG_")) {
                        count++;
                    }
                    var type = getCompType(comp.name.toLowerCase());
                    if (type.matchWith("transition") && layer.name.matchWith("BG_")) {
                        BG = true;
                    }
                    if (type.matchWith("transition") && layer.name.matchWith("FG_")) {
                        FG = true;
                    }
                    if (!(layer instanceof TextLayer) && !layer.source && !(layer.source instanceof CompItem)) {
                        Layer = true;
                    }
                }
                return { count: count, BG: BG, FG: FG, Layer: Layer };
            }

            function getCompMarkers(comp, type) {
                // access comp markers through expressions
                var markers = {}, tempNull = comp.layers.addNull(comp.duration), tempPos = tempNull.property("ADBE Transform Group").property("ADBE Position");
                tempPos.expression = "x = thisComp.marker.numKeys;[x,0];";
                var result = tempPos.value[0];
                if (type == 1) {
                    tempNull.source.remove();
                    return result;
                } else {
                    for (var i = 1; i <= result; i++) {
                        var tempText = comp.layers.addText();
                        var pos = tempText.property("ADBE Transform Group").property("ADBE Position");
                        pos.expression = "[thisComp.marker.key(" + i + ").time,0];";
                        tempText.property("ADBE Text Properties").property("ADBE Text Document").expression = "thisComp.marker.key(" + i + ").comment;";
                        var comment = tempText.property("ADBE Text Properties").property("ADBE Text Document").value.text;
                        var time = tempText.property("ADBE Transform Group").property("ADBE Position").value[0];
                        comment = comment.replace(/(\r\n|\n|\r)/gm, " ");
                        markers = { time: time, comment: comment };

                        tempText.property("ADBE Text Properties").property("ADBE Text Document").expression = "thisComp.marker.key(" + i + ").duration;";
                        var duration = tempText.property("ADBE Text Properties").property("ADBE Text Document").value.text;
                        markers.duration = duration;

                        tempText.property("ADBE Text Properties").property("ADBE Text Document").expression = "thisComp.marker.key(" + i + ").chapter;";
                        var chapter = tempText.property("ADBE Text Properties").property("ADBE Text Document").value.text;
                        markers.chapter = chapter;

                        tempText.property("ADBE Text Properties").property("ADBE Text Document").expression = "thisComp.marker.key(" + i + ").cuePointName;";
                        var cuePointName = tempText.property("ADBE Text Properties").property("ADBE Text Document").value.text;
                        markers.cuePointName = cuePointName;

                        tempText.property("ADBE Text Properties").property("ADBE Text Document").expression = "thisComp.marker.key(" + i + ").eventCuePoint;";
                        var eventCuePoint = tempText.property("ADBE Text Properties").property("ADBE Text Document").value.text;
                        markers.eventCuePoint = eventCuePoint;

                        tempText.property("ADBE Text Properties").property("ADBE Text Document").expression = "thisComp.marker.key(" + i + ").url;";
                        var url = tempText.property("ADBE Text Properties").property("ADBE Text Document").value.text;
                        markers.url = url;

                        tempText.property("ADBE Text Properties").property("ADBE Text Document").expression = "thisComp.marker.key(" + i + ").frameTarget;";
                        var frameTarget = tempText.property("ADBE Text Properties").property("ADBE Text Document").value.text;
                        markers.frameTarget = frameTarget;
                        jsonObj[comp.name]["Markers"].push(markers);
                        tempText.remove();
                        (function (time) {
                            // save Title\Opener\Lowerthird\overlay marker to png
                            var type = getCompType(comp.name.toLowerCase());
                            if (type.matchWith("title") ||
                                type.matchWith('element') ||
                                type.matchWith('transition') ||
                                type.matchWith('effect')) {
                                saveMarkerFrameToPng(comp, time);
                            }
                        })(time);

                    }
                    tempNull.source.remove();
                }
            }

            function findFolderItemByName(name) {
                var found = false;
                var fArr = [];
                for (i = 1; i <= app.project.numItems; i++) {
                    if (name == "final") {
                        var pFolder = app.project.item(1);
                        if (app.project.item(i).name.toUpperCase() == name.toUpperCase() && app.project.item(i) instanceof FolderItem) {
                            var myFolder = app.project.item(i);
                            fArr.push(myFolder);
                        }
                        if (i == app.project.numItems) {
                            if (fArr.length == 1 && (fArr[0].parentFolder.name == "Root" || fArr[0].parentFolder == pFolder)) {
                                found = fArr[0];
                            }
                        }
                    } else {
                        if (app.project.item(i).name.toUpperCase() == name.toUpperCase() && app.project.item(i) instanceof FolderItem) {
                            var myFolder = app.project.item(i);
                            found = myFolder;
                            break
                        }
                    }
                }
                return found;
            }

            function checkCompName(curFolder) {
                for (var i = 1; i <= curFolder.numItems; i++) {
                    var item = curFolder.item(i);
                    if (item instanceof CompItem) {
                        var type = getCompType(item.name.toLowerCase());
                        if (!type.matchWith("title") &&
                            !type.matchWith("effect") &&
                            !type.matchWith("transition") &&
                            !type.matchWith("element") &&
                            !type.matchWith("split")) {
                            return item
                        }
                    }
                }
                return "";
            }

        }

        function getCompType(str){
            /*var type_map = new Map([
                ['title',['wordart','title','opener','lowerthird','subtitle','endscreen','endcredit','callout')],
                ['transition',('transition','ripple&dissolve','wrap','speedblur','slideshow','glitch','3d','brush','smoke','light','elements','shapes')],
                ['effect',('scanline','starburst','flicker','multi-screen','blur','backgroundblur','distortion','mosaic','lut','filter','overlay','aiportrait','arsticker','audioeffect')],
                ['element',('element','background','arrow','countdown','shape','bubble','vfx')]
            ]);*/
            var title_keyword = ['wordart','title','opener','lowerthird','subtitle','endscreen','endcredit','callout'];
            var transition_keyword = ['transition','ripple&dissolve','wrap','speedblur','slideshow','glitch','3d','brush','smoke','light','elements','shapes'];
            var effect_keyword = ['scanline','starburst','flicker','multi-screen','blur','backgroundblur','distortion','mosaic','lut','filter','overlay','aiportrait','arsticker','audioeffect'];
            var element_keyword = ['element','background','arrow','countdown','shape','bubble','vfx'];
            var split_keyword = ['split'];
            var proj_name = app.project.file.name.split(".")[0];
            proj_name = proj_name.split("%20").join(" ");
            str = str.substr(proj_name.length + 1);
            for (var i = 0; i < title_keyword.length; i++) {
                if(str.indexOf(title_keyword[i]) == 0 && str[title_keyword[i].length] == ' ') {
                    return "title";
                }
            }
            for (var i = 0; i < transition_keyword.length; i++) {
                if (str.indexOf(transition_keyword[i]) == 0 && str[transition_keyword[i].length] == ' ') {
                    return "transition";
                }    
            }
            for (var i = 0; i < effect_keyword.length; i++) {
                if (str.indexOf(effect_keyword[i]) == 0 && str[effect_keyword[i].length] == ' ') {
                    return "effect";
                }    
            }
            for (var i = 0; i < element_keyword.length; i++) {
                if (str.indexOf(element_keyword[i]) == 0 && str[element_keyword[i].length] == ' ') {
                    return "element";
                }    
            }
            for(var i=0; i<split_keyword.length; i++){
                if (str.indexOf(split_keyword[i]) == 0 && str[split_keyword[i].length] == ' ') {
                    return "split";
                }  
            }
            return "unknow";
        }

        function getSettingsPath() {
            var path = 'C:/Users/Public/.WEC.CACHE/Settings.json';
            var file = File(path);

            if (!file.exists) {
                file = new File(path);
            }
            return file;
        }

        function getLogPath() {
            var path = 'C:/Users/Public/.WEC.CACHE/debug.log';
            var file = File(path);

            if (!file.exists) {
                file = new File(path);
            }
            return file;
        }

        function dealwithPath(str, flag) {
            var path;
            if (flag == 0) {
                path = str.toString().substring(str.toString().indexOf("/") + 1);
            } else {
                path = str.toString().substring(str.toString().indexOf("/") + 1, str.toString().lastIndexOf("/") + 1);
            }
            var path = path.replace("/", ":/");
            path = path.split("%20").join(" ");
            path = path.split('/').join('//');
            return path;
        }
        function readFile(file) {
            file.open('r', undefined, undefined);
            var obj;
            if (file !== '') {
                var read = file.read();
                var content = read ? read : '{}';
                obj = JSON.parse(content);
                file.close();
            }
            return obj;
        }
        function createFolder(path) {
            var folder = new Folder(path);
            if (!folder.exists) {
                folder.create();
            }
            return folder;
        }

        onSetupEffects();
    }
    parse_ae_elements(this);
}
