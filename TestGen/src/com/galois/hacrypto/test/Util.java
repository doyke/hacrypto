package com.galois.hacrypto.test;

import java.io.File;
import java.io.IOException;
import java.io.PrintWriter;
import java.nio.ByteBuffer;
import java.util.Random;

import org.stringtemplate.v4.ST;

public class Util {

	public static final String VERSION_STRING = "1.0";
	public static final Random rand = new Random();

	/**
	 * Same as {@link Util#writeStringToOutDir(String, String, String)} but uses
	 * the built in file writing of a stringtemplate
	 * 
	 * @param filename
	 *            name of the file to be created/overwritten
	 * @param outputDirectory
	 *            directory to write the file in
	 * @param toWrite
	 *            StringTemplate to render to a file
	 */
	public static void writeSTToOutDir(String filename, String outputDirectory,
			ST toWrite) {
		File outfile = new File(outputDirectory + File.separator + filename);

		try {
			outfile.createNewFile();
		} catch (IOException e) {
			System.err.println("could not create file "
					+ outfile.getAbsolutePath());
			e.printStackTrace();
		}

		try {
			toWrite.write(outfile, null);// TODO: figure out what to do for
											// second argument
		} catch (IOException e) {
			System.err.println("Problem writing to file "
					+ outfile.getAbsolutePath());
			e.printStackTrace();
		}
	}

	/**
	 * @param filename
	 *            name of the file to be created/overwritten
	 * @param outputDirectory
	 *            directory to write the file in
	 * @param toWrite
	 *            String to be written to the file
	 */
	public static void writeStringToOutDir(String filename,
			String outputDirectory, String toWrite) {
		File outfile = new File(outputDirectory + File.separator + filename);
		try {
			outfile.createNewFile();
		} catch (IOException e) {
			System.err.println("could not create file "
					+ outfile.getAbsolutePath());
			e.printStackTrace();
		}

		try {
			PrintWriter out = new PrintWriter(outfile);
			out.print(toWrite);
			out.close();
		} catch (IOException e) {
			System.err.println("Problem writing to file "
					+ outfile.getAbsolutePath());
			e.printStackTrace();
		}
	}

	/**
	 * from <a href =
	 * http://stackoverflow.com/questions/140131/convert-a-string-
	 * representation-of-a-hex-dump-to-a-byte-array-using-java/140861#140861>
	 * this stackoverflow post </a>
	 * 
	 * @param s
	 *            String representation of a hedicamal number. Method will throw
	 *            an exception if the string has odd length
	 * @return Byte array represented by s
	 */
	public static byte[] hexStringToByteArray(String s) {
		int len = s.length();
		if (len % 2 != 0) {
			throw new IllegalArgumentException(
					"Invalid hex string length. Must be even, is " + len);
		}
		byte[] data = new byte[len / 2];
		for (int i = 0; i < len; i += 2) {
			data[i / 2] = (byte) ((Character.digit(s.charAt(i), 16) << 4) + Character
					.digit(s.charAt(i + 1), 16));
		}
		return data;
	}

	/**
	 * The opposite of this function is {@link #intToByteArray(int)}
	 * @param b byte array
	 * @return integer represented by the bytes (big endian)
	 * @throws NumberFormatException if the length of b is greater than 4
	 */
	public static int byteArrayToInt(byte[] b) throws NumberFormatException{
		if(b.length > 4){
			throw new NumberFormatException("byte array to long for conversion to int");
		}
		return ByteBuffer.wrap(b).getInt();
	}
	
	/**
	 * The opposite of this function is {@link #byteArrayToInt(byte[])}
	 * @param i integer to turn to a byte array
	 * @return byte array represented of the integer
	 */
	public static byte[] intToByteArray(int i){
		return ByteBuffer.allocate(4).putInt(i).array();
	}
	
	/**
	 * @param arrayRep
	 *            comma separated string of base 10 bytes. Arrays.toString() of
	 *            a byte array will create valid input to this method
	 * @return byte array represented by arrayRep
	 */
	public static byte[] parseByteArray(String arrayRep) {
		String[] strings = arrayRep.replace("[", "").replace("]", "")
				.split(",");
		byte[] bytes = new byte[strings.length];
		if (strings[0].equals(" ")) {
			bytes = new byte[0];
		} else {
			for (int i = 0; i < strings.length; i++) {
				bytes[i] = Byte.parseByte(strings[i].trim());
			}
		}
		return bytes;
	}

	/**
	 * @param arrayRep
	 *            comma separated string of ints. Arrays.toString() of a byte
	 *            array will create valid input to this method
	 * @return byte array represented by arrayRep
	 */
	public static int[] parseIntArray(String arrayRep) {
		String[] strings = arrayRep.replace("[", "").replace("]", "")
				.split(",");
		int[] ints = new int[strings.length];
		if (strings[0].equals(" ")) {
			ints = new int[0];
		} else {
			for (int i = 0; i < strings.length; i++) {
				ints[i] = Integer.parseInt(strings[i].trim());
			}
		}
		return ints;
	}

	/**
	 * @param bytes
	 * @return String containing a hex representation of the byte array
	 */
	public static String byteArrayToHexString(byte[] bytes) {
		StringBuilder sb = new StringBuilder();
		for (byte b : bytes) {
			sb.append(String.format("%02x", b));
		}
		return sb.toString();
	}

	/**
	 * Compute the bitwise XOR of two arrays of bytes. The arrays have to be of
	 * same length. No length checking is performed.
	 * 
	 * @param x1
	 *            the first array
	 * @param x2
	 *            the second array
	 * @return x1 XOR x2
	 */
	public static byte[] xor(byte[] x1, byte[] x2) {
		byte[] out = new byte[x1.length];

		for (int i = x1.length - 1; i >= 0; i--) {
			out[i] = (byte) (x1[i] ^ x2[i]);
		}
		return out;
	}

	/**
	 * Adjusts the parity of the specified byte array.
	 * @param bytes
	 */
	public static void adjustParity(byte[] bytes) {
		for (int i = 0; i < bytes.length; i++) {
			int b = bytes[i];
			bytes[i] = (byte)((b & 0xfe) | ((((b >> 1) ^ (b >> 2) ^ (b >> 3) ^ (b >> 4) ^ 
					(b >> 5) ^ (b >> 6) ^ (b >> 7)) ^ 0x01) & 0x01));
		}
	}
	
	public static void increment(byte[] bytes) {
		int c = bytes.length - 1;
		do {
			bytes[c] = (byte) (bytes[c] + 1);
			c--;
		} while (c > 0 && bytes[c + 1] == 0x00);
	}

	public static void main(String args[]) {
		byte[] b = new byte[5];
		b[4] = (byte) 0xFF;
		for (int i = 0; i < 20000; i++) {
			System.out.println(byteArrayToHexString(b));
			increment(b);
		}
	}
}
