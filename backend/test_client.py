#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import socket
import json
import sys

# Force UTF-8 output
sys.stdout.reconfigure(encoding='utf-8')

def test_get_exam():
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect(('localhost', 8081))

    request = 'CONTROL GET_EXAM_FOR_STUDENT\n{"exam_id":1,"user_id":13}'
    sock.sendall(request.encode('utf-8'))

    # Receive all data
    response = b''
    sock.settimeout(2)
    try:
        while True:
            chunk = sock.recv(4096)
            if not chunk:
                break
            response += chunk
    except socket.timeout:
        pass

    response = response.decode('utf-8')
    sock.close()

    print("=== RAW RESPONSE ===")
    print(response[:500])  # Print first 500 chars only
    print("\n=== RESPONSE LENGTH ===")
    print(len(response))

    # Try to find JSON part
    json_start = response.find('{')
    if json_start != -1:
        json_str = response[json_start:]
        print("\n=== JSON PART ===")
        print(json_str)
        print("\n=== CHAR AT POSITION 1115 ===")
        if len(json_str) > 1115:
            print(f"Position 1115: '{json_str[1115]}' (ord={ord(json_str[1115])})")
            print(f"Context: ...{json_str[1100:1130]}...")

        try:
            data = json.loads(json_str)
            print("\n=== PARSED OK ===")
            print(f"Questions: {len(data.get('questions', []))}")
            print(f"Submission: {data.get('submission')}")
        except json.JSONDecodeError as e:
            print(f"\n=== JSON PARSE ERROR ===")
            print(f"Error: {e}")
            print(f"Position: {e.pos}")
            if e.pos:
                print(f"Context: ...{json_str[max(0, e.pos-50):e.pos+50]}...")

if __name__ == '__main__':
    test_get_exam()
