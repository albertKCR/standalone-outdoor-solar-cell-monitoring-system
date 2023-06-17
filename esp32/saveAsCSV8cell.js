function doGet(e){
  return saveAsCSV();
}
function saveAsCSV() {
  var sheet = SpreadsheetApp.openById('1ELHCl-1dL5LFJFQ-sP02MA2dvbyNQNiFwArYep-9QsU').getSheetByName("Sheet1");
  var lastRow = sheet.getLastRow();
  var range = sheet.getRange("C2:D"+lastRow);
  var csv = "";
  var rows = range.getValues();
  for (var i = 0; i < rows.length; i++) {
    csv += rows[i].join(",") + "\n";
  }
  var folder = DriveApp.getFolderById("1nt-Px6BmoRV0BHB6BhUHxzda2JH02xVT");
  var fileName = Utilities.formatDate(new Date(), 'America/Sao_Paulo', "dd/MM/yyyy") + " - CELL 1";
  //var fileName = sheet.getRange("A2").getValue().toString().split(" ").slice(0,4).join("_");
  var file = DriveApp.createFile(fileName + ".csv", csv, MimeType.CSV);
  folder.addFile(file);

  var range = sheet.getRange("E2:F"+lastRow);
  var csv = "";
  var rows = range.getValues();
  for (var i = 0; i < rows.length; i++) {
    csv += rows[i].join(",") + "\n";
  }
  var folder = DriveApp.getFolderById("1nt-Px6BmoRV0BHB6BhUHxzda2JH02xVT");
  var fileName = Utilities.formatDate(new Date(), 'America/Sao_Paulo', "dd/MM/yyyy") + " - CELL 2";
  //var fileName = sheet.getRange("A2").getValue().toString().split(" ").slice(0,4).join("_");
  var file = DriveApp.createFile(fileName + ".csv", csv, MimeType.CSV);
  folder.addFile(file);

  var range = sheet.getRange("G2:H"+lastRow);
  var csv = "";
  var rows = range.getValues();
  for (var i = 0; i < rows.length; i++) {
    csv += rows[i].join(",") + "\n";
  }
  var folder = DriveApp.getFolderById("1nt-Px6BmoRV0BHB6BhUHxzda2JH02xVT");
  var fileName = Utilities.formatDate(new Date(), 'America/Sao_Paulo', "dd/MM/yyyy") + " - CELL 3";
  //var fileName = sheet.getRange("A2").getValue().toString().split(" ").slice(0,4).join("_");
  var file = DriveApp.createFile(fileName + ".csv", csv, MimeType.CSV);
  folder.addFile(file);
  
  var range = sheet.getRange("I2:J"+lastRow);
  var csv = "";
  var rows = range.getValues();
  for (var i = 0; i < rows.length; i++) {
    csv += rows[i].join(",") + "\n";
  }
  var folder = DriveApp.getFolderById("1nt-Px6BmoRV0BHB6BhUHxzda2JH02xVT");
  var fileName = Utilities.formatDate(new Date(), 'America/Sao_Paulo', "dd/MM/yyyy") + " - CELL 4";
  //var fileName = sheet.getRange("A2").getValue().toString().split(" ").slice(0,4).join("_");
  var file = DriveApp.createFile(fileName + ".csv", csv, MimeType.CSV);
  folder.addFile(file);

  var range = sheet.getRange("K2:L"+lastRow);
  var csv = "";
  var rows = range.getValues();
  for (var i = 0; i < rows.length; i++) {
    csv += rows[i].join(",") + "\n";
  }
  var folder = DriveApp.getFolderById("1nt-Px6BmoRV0BHB6BhUHxzda2JH02xVT");
  var fileName = Utilities.formatDate(new Date(), 'America/Sao_Paulo', "dd/MM/yyyy") + " - CELL 5";
  //var fileName = sheet.getRange("A2").getValue().toString().split(" ").slice(0,4).join("_");
  var file = DriveApp.createFile(fileName + ".csv", csv, MimeType.CSV);
  folder.addFile(file);

  var range = sheet.getRange("M2:N"+lastRow);
  var csv = "";
  var rows = range.getValues();
  for (var i = 0; i < rows.length; i++) {
    csv += rows[i].join(",") + "\n";
  }
  var folder = DriveApp.getFolderById("1nt-Px6BmoRV0BHB6BhUHxzda2JH02xVT");
  var fileName = Utilities.formatDate(new Date(), 'America/Sao_Paulo', "dd/MM/yyyy") + " - CELL 6";
  //var fileName = sheet.getRange("A2").getValue().toString().split(" ").slice(0,4).join("_");
  var file = DriveApp.createFile(fileName + ".csv", csv, MimeType.CSV);
  folder.addFile(file);

  var range = sheet.getRange("O2:P"+lastRow);
  var csv = "";
  var rows = range.getValues();
  for (var i = 0; i < rows.length; i++) {
    csv += rows[i].join(",") + "\n";
  }
  var folder = DriveApp.getFolderById("1nt-Px6BmoRV0BHB6BhUHxzda2JH02xVT");
  var fileName = Utilities.formatDate(new Date(), 'America/Sao_Paulo', "dd/MM/yyyy") + " - CELL 7";
  //var fileName = sheet.getRange("A2").getValue().toString().split(" ").slice(0,4).join("_");
  var file = DriveApp.createFile(fileName + ".csv", csv, MimeType.CSV);
  folder.addFile(file);

  var range = sheet.getRange("Q2:R"+lastRow);
  var csv = "";
  var rows = range.getValues();
  for (var i = 0; i < rows.length; i++) {
    csv += rows[i].join(",") + "\n";
  }
  var folder = DriveApp.getFolderById("1nt-Px6BmoRV0BHB6BhUHxzda2JH02xVT");
  var fileName = Utilities.formatDate(new Date(), 'America/Sao_Paulo', "dd/MM/yyyy") + " - CELL 8";
  //var fileName = sheet.getRange("A2").getValue().toString().split(" ").slice(0,4).join("_");
  var file = DriveApp.createFile(fileName + ".csv", csv, MimeType.CSV);
  folder.addFile(file);
}
